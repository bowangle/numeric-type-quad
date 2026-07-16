cmake_minimum_required(VERSION 3.5)
project(TCIcpp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_INCLUDE_DIR     ${CMAKE_SOURCE_DIR}/include)
set(EIGEN_INCLUDE_DIR       ${CMAKE_SOURCE_DIR}/extern/eigen)
set(QD_INCLUDE_DIR          ${CMAKE_SOURCE_DIR}/extern/qd-install/include)
set(QD_LIBRARY_DIR          ${CMAKE_SOURCE_DIR}/extern/qd-install/lib)

add_executable(Test test/test_type.cpp)

# ---- include dirs (same for all targets) ----------------------------------------
foreach(_tgt Test)
    target_include_directories(${_tgt} PUBLIC
        ${PROJECT_INCLUDE_DIR}
        ${EIGEN_INCLUDE_DIR}
        ${QD_INCLUDE_DIR}
    )
    target_link_directories(${_tgt} PRIVATE ${QD_LIBRARY_DIR})
    target_link_libraries(${_tgt} PRIVATE qd quadmath)
    target_compile_options(${_tgt} PRIVATE -Wall -Wextra -Wreorder -Wshadow)
endforeach()