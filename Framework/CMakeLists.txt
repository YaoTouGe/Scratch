project(Framework)

include_directories(Common)
include_directories(Interface)
include_directories(Apps)
include_directories(../Graphics)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

file(GLOB MY_SOURCE_FILES Common/*.cpp Apps/*.cpp)

add_executable(app ${MY_SOURCE_FILES})
target_link_libraries(app glfw Graphics ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})