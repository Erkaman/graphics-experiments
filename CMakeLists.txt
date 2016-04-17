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
file(GLOB ENGINE_SRC ewa/*.cpp ewa/*.c ewa/gl/shader/*.cpp ewa/gl/*.cpp ewa/math/*.cpp ewa/audio/*.cpp  deps/src/lodepng/*.cpp deps/src/imgui/*.cpp deps/glad/src/*.c )

######################################
############ ADD  LIBRARIES
######################################

FIND_PACKAGE(OpenGL REQUIRED)

LIST(APPEND game_libs ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

LIST(APPEND game_libs glfw3)

IF(APPLE)

  LIST(APPEND ENGINE_SRC deps/src/nfd/nfd_common.c deps/src/nfd/nfd_cocoa.m samples/game/src/cocoa_util.m)


  LINK_DIRECTORIES("deps/libs/mac")
  find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
  find_library(OPENGL_FRAMEWORK OpenGl)

  find_library(COCOA_FRAMEWORK Cocoa)
  find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
    find_library(IOKIT_FRAMEWORK IOKit)

  LIST(APPEND game_libs "${OPENGL_FRAMEWORK}" "${CORE_FOUNDATION_FRAMEWORK}"
    "${COCOA_FRAMEWORK}"
    "${CORE_VIDEO_FRAMEWORK}"
    "${IOKIT_FRAMEWORK}")

	FIND_PACKAGE(OpenAL REQUIRED)
	LIST(APPEND game_libs ${OPENAL_LIBRARY})

	FIND_PACKAGE(Bullet REQUIRED)
	LIST(APPEND game_libs ${BULLET_LIBRARIES})
	include_directories( ${BULLET_INCLUDE_DIRS} )


ELSEIF(UNIX)
  LINK_DIRECTORIES("deps/libs/linux")
  find_package(X11 REQUIRED)
  find_package(Threads REQUIRED)
  list(APPEND game_libs "${X11_X11_LIB}" "${RT_LIBRARY}" "${X11_Xrandr_LIB}" "${X11_Xinput_LIB}" "${X11_Xcursor_LIB}" "${X11_Xinerama_LIB}" "${X11_Xxf86vm_LIB}" "${CMAKE_THREAD_LIBS_INIT}")

	FIND_PACKAGE(Bullet REQUIRED)
	LIST(APPEND game_libs ${BULLET_LIBRARIES})
	include_directories( ${BULLET_INCLUDE_DIRS} )

	FIND_PACKAGE(OpenAL REQUIRED)
	LIST(APPEND game_libs ${OPENAL_LIBRARY})


ELSEIF(WIN32)

  LINK_DIRECTORIES("deps/libs/win32")

  # add openal
  LIST(APPEND game_libs OpenAL32)

  # add bullet
  LIST(APPEND game_libs BulletDynamics BulletCollision LinearMath)
  include_directories("deps/include/bullet" )




ENDIF()

include_directories("deps/include" "ewa" "ewa/gl" "ewa/gl/shader" "deps/include/lodepng" "."  "deps/include/imgui"
  "deps/include/nfd" "deps/glad/include")

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(CreateLaunchers)

ADD_LIBRARY(ewa ${ENGINE_SRC})
TARGET_LINK_LIBRARIES(ewa ${game_libs})

add_subdirectory("samples")

add_subdirectory("unit_test")