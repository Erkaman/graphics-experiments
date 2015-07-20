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
ADD_DEFINITIONS(/GR- /MTd /W4)

else()
ADD_DEFINITIONS( -pedantic -Wno-deprecated -fno-exceptions -fno-rtti -Waddress
-Warray-bounds
-Wc++11-compat
-Wchar-subscripts
-Wcomment
-Wformat
-Wnonnull
-Wparentheses
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
file(GLOB ENGINE_SRC ewa/*.cpp ewa/gl/shader/*.cpp ewa/gl/*.cpp ewa/math/*.cpp deps/src/glew/*.c deps/src/lodepng/*.cpp )


######################################
############ ADD  LIBRARIES
######################################


FIND_PACKAGE(OpenGL REQUIRED)
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
  find_package(Threads REQUIRED)
  list(APPEND game_libs "${X11_X11_LIB}" "${RT_LIBRARY}" "${X11_Xrandr_LIB}" "${X11_Xinput_LIB}" "${X11_Xcursor_LIB}" "${X11_Xinerama_LIB}" "${X11_Xxf86vm_LIB}" "${CMAKE_THREAD_LIBS_INIT}")
ELSEIF(WIN32)
  MESSAGE(linking)
  LINK_DIRECTORIES("deps/libs/win32")
ENDIF()

file(GLOB TEST_SRC unit_test/*.cpp)

include_directories("deps/include" "ewa" "ewa/gl" "ewa/gl/shader" "deps/include/lodepng")

ADD_LIBRARY(ewa ${ENGINE_SRC})
TARGET_LINK_LIBRARIES(ewa ${game_libs})

add_subdirectory("samples")


ADD_EXECUTABLE(unit_test ${TEST_SRC} )
TARGET_LINK_LIBRARIES(unit_test ewa)
