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

find_package(OpenGL REQUIRED)

# Add source files
add_executable(visualizer ${SOURCES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # On windows, we move glew32.dll into the build folder
    file(COPY ${GLEW_ROOT_DIR}/bin/Release/x64/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
    set(GLEW_LIB ${CMAKE_SOURCE_DIR}/lib/glew32.lib)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLEW_LIB GLEW)
    target_link_libraries(visualizer m)
else ()
endif ()

target_link_libraries(visualizer glfw)
target_link_libraries(visualizer ${GLEW_LIB})
target_link_libraries(visualizer OpenGL::GL)


