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

#include(cmake/PrecompiledHeader.cmake)

#if(MSVC)
#  set(pch_source test-pch.cpp)
#endif()

#SET(SRC main.cpp)
file(GLOB SRC src/*.cpp src/gl/shader/*.cpp src/gl/*.cpp deps/src/glew/*.c )


######################################
############ ADD  LIBRARIES
######################################


FIND_PACKAGE(OpenGL)
LIST(APPEND game_libs ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

LIST(APPEND game_libs glfw3)

IF(APPLE)
  MESSAGE(hi)
  LINK_DIRECTORIES("deps/libs/mac")
  find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
  find_library(COCOA_FRAMEWORK Cocoa)
  find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
    find_library(IOKIT_FRAMEWORK IOKit)

  LIST(APPEND game_libs "${CORE_FOUNDATION_FRAMEWORK}"
    "${COCOA_FRAMEWORK}"
    "${CORE_VIDEO_FRAMEWORK}"
    "${IOKIT_FRAMEWORK}")

ENDIF()

ADD_EXECUTABLE(game ${SRC} ${pch_source})

#add_precompiled_header(game src/precomp.hpp)




include_directories("deps/include" "src" "src/gl" "src/gl/shader")

TARGET_LINK_LIBRARIES(game ${game_libs})
