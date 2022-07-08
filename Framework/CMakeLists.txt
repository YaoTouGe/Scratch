include_directories(Common)
include_directories(Interface)

find_package(GLFW)

file(GLOB MY_SOURCE_FILES Common/*.cpp)
add_executable(app ${MY_SOURCE_FILES})