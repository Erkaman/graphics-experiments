#pragma once

#include "gl_common.hpp"
#include <string>

class Texture {

private:

    Texture(const Texture&);
    Texture& operator=(const Texture&);


    static GLfloat cachedMaxAnisotropic;

    bool m_alreadyBound;

protected:

    GLuint m_textureHandle;

    GLenum m_target;
    GLsizei m_width;
    GLsizei m_height;

public:

    Texture(const GLenum target):m_alreadyBound(false), m_target(target)  {
	GL_C(glGenTextures(1, &m_textureHandle));
    }

    // create an empty texture.
    Texture(const GLenum target, const GLsizei width, const GLsizei height, const GLint internalFormat, const GLenum type);

    ~Texture()  {
	GL_C(glDeleteTextures(1, &m_textureHandle));
    }

    GLsizei GetWidth() const{
        return m_width;
    }

    GLsizei GetHeight() const{
        return m_height;
    }

    GLenum GetTarget() const{
        return m_target;
    }

    GLuint GetHandle() const{
        return m_textureHandle;
    }

    void Bind() {
        if(m_alreadyBound)
            return;

        GL_C(glBindTexture(m_target, m_textureHandle));
        m_alreadyBound = true;
    }

    void Unbind() {
        if(!m_alreadyBound)
            return;

        GL_C(glBindTexture(m_target, 0));
        m_alreadyBound = false;
    }

    operator std::string() const;


    /**
     * Enables texture tiling for this texture.
     */
    void SetTextureTiling() {
        GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }

    /**
     * Enables texture clamping for this texture
     */
    void SetTextureClamping() {
        GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }


    void EnableAnisotropicFiltering() {
        GL_C(glTexParameterf(m_target,GL_TEXTURE_MAX_ANISOTROPY_EXT, GetMaxAnisotropic()));
    }

    static float GetMaxAnisotropic();

    void SetBestMagMinFilters(const bool generateMipmap) {
        if(generateMipmap)
            GenerateMipmap();

        EnableAnisotropicFiltering();

        GL_C(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_C(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    }

    void SetBestMagMinFilters() {
        SetBestMagMinFilters(true);
    }

    void GenerateMipmap() {
    	GL_C(glGenerateMipmap(m_target));
    }

    void SetMagMinFilters(const GLint filter) {
    	SetMagFilter(filter);
    	SetMinFilter(filter);

    }

    void SetMagFilter(const GLint filter) {
        GL_C(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER,  filter));
    }

    void SetMinFilter(const GLint filter) {
        GL_C(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, filter ));
    }

    static void SetActiveTextureUnit(const GLenum textureUnit) {
	GL_C(glActiveTexture(GL_TEXTURE0 + textureUnit));
    }
};
