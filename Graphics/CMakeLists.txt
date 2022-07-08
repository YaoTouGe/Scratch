find_package(Eigen3)

include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB MY_SOURCE_FILES *.cpp)
add_library(Graphics ${MY_SOURCE_FILES})