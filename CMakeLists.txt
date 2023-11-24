cmake_minimum_required(VERSION 3.12)
project(visualizer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Find source files
file(GLOB SOURCES "src/*.cpp")


#if https://github.com/ebajec/linear-algebra is cloned to the same location as
#this project, this should work fine.  If not, make sure to add in here the path 
#to wherever it was cloned. 
include_directories(include ../linear-algebra/lib src)

# Set cache variables for GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#if you built glfw from source files, add the path to the root directory
#for glfw here
set(GLFW_ROOT_DIR ../../../glfw )

#link GLEW differently depending on platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GLEW_LIB ${CMAKE_SOURCE_DIR}/lib/glew32.lib)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLEW_LIB GLEW)
else ()
endif ()

add_subdirectory(${GLFW_ROOT_DIR} build)
file(COPY ${CMAKE_SOURCE_DIR}/windows/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})

find_package(OpenGL REQUIRED)

# Add source files
add_executable(visualizer ${SOURCES})

target_link_libraries(visualizer glfw)
target_link_libraries(visualizer ${GLEW_LIB})
target_link_libraries(visualizer OpenGL::GL)

