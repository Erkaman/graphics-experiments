#pragma once

#include <vector>
#include <string>
#include "gl_common.hpp"

struct TextureInfo{
    GLint internalFormat;
    GLenum format;
    GLsizei width;
    GLsizei height;
    GLenum type;

    std::vector<unsigned char> imageData;
};

class TextureLoader {

public:

    static TextureInfo Load(const std::string& texturePath);

};
