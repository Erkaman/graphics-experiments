######################################
#### PROJECT SETTINGS.
######################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT(game)

# COMPILER SETTINGS.
if(MSVC)
else()
ADD_DEFINITIONS(-Wall -pedantic -Wno-deprecated -fno-exceptions -fno-rtti)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  ")
endif()

######################################
#### SET SOURCE FILES.
######################################

include(cmake/PrecompiledHeader.cmake)

if(MSVC)
  set(pch_source test-pch.cpp)
endif()

#SET(SRC main.cpp)
file(GLOB SRC src/*.cpp src/gl/shader/*.cpp src/gl/*.cpp deps/src/glew/*.c )

ADD_EXECUTABLE(game ${SRC} ${pch_source} src/precomp.hpp)

add_precompiled_header(game src/precomp.hpp)

######################################
############ ADD  LIBRARIES
######################################

FIND_PACKAGE(OpenGL)

SET(GL_LIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY})


include_directories("deps/include" "src" "src/gl" "src/gl/shader")
LINK_DIRECTORIES("deps/libs")
TARGET_LINK_LIBRARIES(game glfw3.3.1 ${GL_LIBS})
