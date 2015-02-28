#pragma once

#define _DEBUG

/*
  OpenGL stuff.
 */
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library


/*
  C standard library:
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

//  STL.

#include <string>
#include <map>
#include <vector>
#include <memory>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/*
  Common utility functions:
 */
#include "gl/glutil.h"
#include "log.h"
#include "str.h"
