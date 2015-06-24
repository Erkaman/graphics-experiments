######################################
#### PROJECT SETTINGS.
######################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
PROJECT(game)

SET(CMAKE_BUILD_TYPE "Debug")

add_definitions(-DGLEW_STATIC)

# COMPILER SETTINGS.
if(MSVC)
MESSAGE(hi)
ADD_DEFINITIONS(/GR- /MTd)

else()
ADD_DEFINITIONS( -pedantic -Wno-deprecated -fno-exceptions -fno-rtti -Waddress
-Warray-bounds
-Wc++11-compat
-Wchar-subscripts
-Wcomment
-Wformat
-Wnonnull
-Wparentheses
-Wpointer-sign
-Wreorder
-Wreturn-type
-Wsequence-point
-Wsign-compare
-Wstrict-aliasing
-Wstrict-overflow=1
-Wswitch
-Wtrigraphs
-Wuninitialized
-Wunused-function
-Wunused-label
-Wunused-value
-Wunused-variable
-Wvolatile-register-var)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
endif()

######################################
#### SET SOURCE FILES.
######################################

#SET(SRC main.cpp)
file(GLOB SRC src/*.cpp src/gl/shader/*.cpp src/gl/*.cpp deps/src/glew/*.c )


######################################
############ ADD  LIBRARIES
######################################


FIND_PACKAGE(OpenGL)
LIST(APPEND game_libs ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

LIST(APPEND game_libs glfw3)

IF(APPLE)
  LINK_DIRECTORIES("deps/libs/mac")
  find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
  find_library(COCOA_FRAMEWORK Cocoa)
  find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
    find_library(IOKIT_FRAMEWORK IOKit)

  LIST(APPEND game_libs "${CORE_FOUNDATION_FRAMEWORK}"
    "${COCOA_FRAMEWORK}"
    "${CORE_VIDEO_FRAMEWORK}"
    "${IOKIT_FRAMEWORK}")
ELSEIF(UNIX)
  LINK_DIRECTORIES("deps/libs/linux")
  find_package(X11 REQUIRED)
  list(APPEND game_libs "${X11_X11_LIB}" "${RT_LIBRARY}" "${X11_Xrandr_LIB}" "${X11_Xinput_LIB}" "${X11_Xcursor_LIB}" "${X11_Xinerama_LIB}")
ELSEIF(WIN32)
  MESSAGE(linking)
  LINK_DIRECTORIES("deps/libs/win32")
ENDIF()

ADD_EXECUTABLE(game ${SRC})

include_directories("deps/include" "src" "src/gl" "src/gl/shader")

TARGET_LINK_LIBRARIES(game ${game_libs})