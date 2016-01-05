#pragma once

#pragma warning( disable : 4996 )
#pragma warning( disable : 4127 )


#include <string>

template <typename T>
std::string tos(const T& t) {
	return std::string(t);
}

template <typename T>
const char* tocstr(const T& t) {
    return std::string(t).c_str();
}


#include "log.hpp"

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;

typedef unsigned long long uint64;

#define MY_DELETE(p)      if ( (p) != NULL ) { delete p; p = NULL; }

#define MY_DELETEA(p)     if ( (p) ) { delete [] (p); p = NULL; }

constexpr float WORLD_SCALE = 1.0f;
