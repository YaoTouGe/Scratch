project(Graphics)

find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB MY_SOURCE_FILES *.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(Graphics ${MY_SOURCE_FILES})