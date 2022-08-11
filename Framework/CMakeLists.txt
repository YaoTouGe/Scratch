project(Framework)

find_package(OpenGL REQUIRED)

file(GLOB MY_SOURCE_FILES Common/*.cpp)

add_executable(scratch ${MY_SOURCE_FILES})
target_link_libraries(scratch glfw glew Apps Graphics ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})