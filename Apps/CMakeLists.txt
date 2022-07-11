project(Apps)

file(GLOB SRC_FILES *.cpp)
add_library(Apps ${SRC_FILES})