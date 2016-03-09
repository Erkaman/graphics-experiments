#pragma once

#include "texture.hpp"

class DdsInfo;

class Texture2D : public Texture{

private:
       GLint m_level;

    GLenum m_format;
    GLenum m_type;

public:


    static Texture2D* Load(const std::string& texturePath);

    Texture2D(GLvoid* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum type);

    Texture2D(DdsInfo* di);

    void UpdateTexture(GLvoid* data);
};
