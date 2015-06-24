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

void Error(const std::string& errorStr);
