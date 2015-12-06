#pragma once

#include "texture.hpp"

class Texture2D : public Texture{

public:


//    Texture2D(const std::string& texturePath);

    static Texture2D* Load(const std::string& texturePath);

    Texture2D(GLvoid* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum type);



};
