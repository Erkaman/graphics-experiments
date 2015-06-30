#pragma once

#pragma warning( disable : 4996 )
#pragma warning( disable : 4127 )


#include <memory>
#include <string>

#ifndef _WIN32

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


#endif

template <typename T>
std::string tos(const T& t) {
	return std::string(t);
}

#include "log.hpp"

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int int32;
typedef short int16;
typedef char int8;
