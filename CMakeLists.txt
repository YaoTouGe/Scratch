cmake_minimum_required (VERSION 3.1)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 11)
project(Scratch)

find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(3rd)
include_directories(Graphics)
include_directories(Framework)

add_subdirectory(Framework)
add_subdirectory(Graphics)
add_subdirectory(Apps)