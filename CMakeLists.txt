cmake_minimum_required(VERSION 3.21)
project(numeric-type-quad VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Boost is system-provided; Eigen and the already-built QD come from extern/.
find_package(Boost REQUIRED)
set(NUMERIC_TYPE_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../extern"
    CACHE PATH "Shared Eigen and QD dependencies")
find_path(EIGEN_INCLUDE_DIR Eigen/Core PATHS "${NUMERIC_TYPE_EXTERN_DIR}/eigen"
    NO_DEFAULT_PATH NO_CACHE REQUIRED)
find_path(QD_INCLUDE_DIR qd/dd_real.h PATHS "${NUMERIC_TYPE_EXTERN_DIR}/qd-install/include"
    NO_DEFAULT_PATH NO_CACHE REQUIRED)
find_library(QD_LIBRARY NAMES qd PATHS
    "${NUMERIC_TYPE_EXTERN_DIR}/qd-install/lib" "${NUMERIC_TYPE_EXTERN_DIR}/qd-install/lib64"
    NO_DEFAULT_PATH NO_CACHE REQUIRED)

option(BUILD_SHARED_LIBS "Build shared instead of static libraries" OFF)
option(NUMERIC_TYPE_BUILD_TESTS "Build numeric tests" ${PROJECT_IS_TOP_LEVEL})
add_library(numeric_type_quad
    src/type_double_double.cpp src/type_float128_boost.cpp src/type_int128.cpp src/pi.cpp)
add_library(numeric_type_quad::numeric_type_quad ALIAS numeric_type_quad)
set_target_properties(numeric_type_quad PROPERTIES
    POSITION_INDEPENDENT_CODE ON INSTALL_RPATH_USE_LINK_PATH TRUE)
target_compile_features(numeric_type_quad PUBLIC cxx_std_20)
target_compile_definitions(numeric_type_quad PUBLIC EIGEN_DEFAULT_DENSE_INDEX_TYPE=int)
target_include_directories(numeric_type_quad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(numeric_type_quad SYSTEM PUBLIC ${EIGEN_INCLUDE_DIR} ${QD_INCLUDE_DIR})
target_link_libraries(numeric_type_quad PUBLIC Boost::headers "${QD_LIBRARY}" quadmath)

if(NUMERIC_TYPE_BUILD_TESTS)
    enable_testing()
    foreach(name type library)
        add_executable(numeric_type_test_${name} test/test_${name}.cpp)
        target_link_libraries(numeric_type_test_${name} PRIVATE numeric_type_quad)
        add_test(NAME numeric_type_${name} COMMAND numeric_type_test_${name})
    endforeach()
endif()

# Install the library, headers, and package consumed by higher layers.
install(TARGETS numeric_type_quad EXPORT numeric-type-quadTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
set(package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/numeric-type-quad")
install(EXPORT numeric-type-quadTargets
    NAMESPACE numeric_type_quad:: DESTINATION "${package_dir}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/numeric-type-quadConfig.cmake" [=[
include(CMakeFindDependencyMacro)
find_dependency(Boost)
include("${CMAKE_CURRENT_LIST_DIR}/numeric-type-quadTargets.cmake")
]=])
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/numeric-type-quadConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/numeric-type-quadConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/numeric-type-quadConfigVersion.cmake"
    DESTINATION "${package_dir}")
