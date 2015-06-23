#include "common.hpp"

std::string tos(const char* cstr) {
    return std::string(cstr);
}

std::string tos(const unsigned char* cstr) {
    return std::string(reinterpret_cast<const char*> (cstr));
}

std::string tos(const int i) {
    return std::to_string(i);
}


void Error(const std::string& errorStr) {
    LOG_E(errorStr);
    exit(1);
}
