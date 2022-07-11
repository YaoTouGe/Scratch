project(Apps)

include_directories(../Graphics)
include_directories(../Framework/Common)
include_directories(../Framework/Interface)

find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB SRC_FILES *.cpp)
add_library(Apps ${SRC_FILES})