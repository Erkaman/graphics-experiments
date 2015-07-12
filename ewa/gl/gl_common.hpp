#pragma once

#pragma warning( disable : 4127 )


#ifndef OPENGL_ALREADY_INCLUDED

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#endif


#include <vector>
#include "gl_util.hpp"

constexpr GLuint VBO_POSITION_ATTRIB_INDEX = 0;
constexpr GLuint VBO_TEX_COORD_ATTRIB_INDEX = 2;
constexpr GLuint VBO_NORMAL_ATTRIB_INDEX = 1;
constexpr GLuint VBO_COLOR_ATTRIB_INDEX = 3;

typedef std::vector<GLushort> UshortVector;
typedef std::vector<GLfloat> FloatVector;
typedef std::vector<GLuint> UintVector;
