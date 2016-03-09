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

struct DdsInfo{
    unsigned int format;
    unsigned int width;
    unsigned height;

    int components;

    unsigned char* buffer;

   std::vector<unsigned char> imageData;

    unsigned int mipMapCount;


};

class TextureLoader {

public:

    static TextureInfo* Load(const std::string& texturePath);

    static DdsInfo* LoadDds(const std::string& texturePath);

};
