cmake_minimum_required(VERSION 3.21)
project(numeric-type-quad LANGUAGES CXX)

# Only meaningful for the developer configuring this layer directly;
# the top-level project sets it for the whole tree.
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

option(NUMERIC_TYPE_BUILD_TESTS
       "Build numeric-type-quad's own tests"
       ${PROJECT_IS_TOP_LEVEL})

# ---- own paths ----------------------------------------------------------------
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EIGEN_INCLUDE_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/extern/eigen)
set(QD_INCLUDE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/extern/qd-install/include)
set(QD_LIBRARY_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/extern/qd-install/lib)

# ---- sanity checks: externals must be present --------------------------------
if(NOT EXISTS ${QD_INCLUDE_DIR}/qd/qd_real.h)
    message(FATAL_ERROR
        "QD not found at ${QD_INCLUDE_DIR}.\n"
        "Run ${CMAKE_CURRENT_SOURCE_DIR}/install_extern.sh first "
        "to build the QD library.")
endif()

if(NOT EXISTS ${EIGEN_INCLUDE_DIR}/Eigen/Core)
    message(FATAL_ERROR
        "Eigen not found at ${EIGEN_INCLUDE_DIR}.\n"
        "Run: git submodule update --init --recursive")
endif()

# ---- imported target for QD ---------------------------------------------------
# NO_DEFAULT_PATH so we can never silently pick up a system libqd that
# disagrees with the headers above. GLOBAL so consumers two or three
# add_subdirectory() levels up can still see qd::qd.
find_library(QD_LIBRARY
    NAMES qd
    PATHS ${QD_LIBRARY_DIR}
    NO_DEFAULT_PATH
    REQUIRED)

add_library(qd::qd UNKNOWN IMPORTED GLOBAL)
set_target_properties(qd::qd PROPERTIES
    IMPORTED_LOCATION             "${QD_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${QD_INCLUDE_DIR}")

# ---- interface target: carries our headers + transitive deps ------------------
add_library(numeric_type_deps INTERFACE)

target_include_directories(numeric_type_deps INTERFACE
    ${PROJECT_INCLUDE_DIR})

# SYSTEM => -isystem, so Eigen's own warnings don't pollute consumers
# built with -Wall -Wextra. (qd::qd's includes are already treated as
# SYSTEM because it is an imported target.)
target_include_directories(numeric_type_deps SYSTEM INTERFACE
    ${EIGEN_INCLUDE_DIR})

target_compile_features(numeric_type_deps INTERFACE cxx_std_20)

target_link_libraries(numeric_type_deps INTERFACE qd::qd)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(numeric_type_deps INTERFACE quadmath)
endif()

# ---- tests (leaf targets: nothing may ever depend on these) -------------------
if(NUMERIC_TYPE_BUILD_TESTS)
    enable_testing()

    add_executable(numeric_type_test test/test_type.cpp)
    target_link_libraries(numeric_type_test PRIVATE numeric_type_deps)
    target_compile_options(numeric_type_test PRIVATE
        -Wall -Wextra -Wreorder -Wshadow)

    add_test(NAME numeric_type_test COMMAND numeric_type_test)
endif()