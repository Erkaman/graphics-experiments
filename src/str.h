#pragma once

// this file contains string utility functions.

// TOString
inline std::string tos(const char* cstr) {
    return std::string(cstr);
}

inline std::string tos(const unsigned char* cstr) {
    return std::string(reinterpret_cast<const char*> (cstr));
}

inline std::string tos(int i) {
    return std::to_string(i);
}
