#pragma once

#include <memory>
#include <string>

#ifndef _WIN32

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

// TOString
std::string tos(const char* cstr);
std::string tos(const unsigned char* cstr);
std::string tos(const int i);

#include "log.hpp"

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;
