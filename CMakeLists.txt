cmake_minimum_required(VERSION 3.16)
project(numeric-type-quad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- own paths ----------------------------------------------------------------
set(PROJECT_INCLUDE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EIGEN_INCLUDE_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/extern/eigen)
set(QD_INCLUDE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/extern/qd-install/include)
set(QD_LIBRARY_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/extern/qd-install/lib)

# ---- sanity check: QD must be built first -------------------------------------
if(NOT EXISTS ${QD_INCLUDE_DIR}/qd/qd_real.h)
    message(FATAL_ERROR
        "QD not found at ${QD_INCLUDE_DIR}.\n"
        "Run install_extern.sh first to build the QD library.")
endif()

# ---- interface target: carries our headers + transitive deps ------------------
add_library(numeric_type_deps INTERFACE)
target_include_directories(numeric_type_deps INTERFACE
    ${PROJECT_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${QD_INCLUDE_DIR}
)
target_link_directories(numeric_type_deps INTERFACE ${QD_LIBRARY_DIR})
target_link_libraries(numeric_type_deps INTERFACE qd quadmath)

# ---- test targets -------------------------------------------------------------
add_executable(Test test/test_type.cpp)
target_link_libraries(Test PRIVATE numeric_type_deps)
target_compile_options(Test PRIVATE -Wall -Wextra -Wreorder -Wshadow)