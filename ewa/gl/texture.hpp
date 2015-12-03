#pragma once

#include "gl_common.hpp"
#include <string>

class Texture {

private:

    /*
      Copying is not possible.
    */
    Texture(const Texture&);
    Texture& operator=(const Texture&);

    /*

      PRIVATE VARIABLES
    */
    static GLfloat cachedMaxAnisotropic;

protected:

    /*
      PROTECTED VARIABLES:
    */
    GLuint m_textureHandle;
    GLenum m_target;
    GLsizei m_width;
    GLsizei m_height;

public:

    /*
      CONSTRUCTORS AND DESTRUCTORS:
    */

    Texture(const GLenum target);

    // create an empty texture.
    Texture(const GLenum target, const GLsizei width, const GLsizei height, const GLint internalFormat, const GLenum format,const GLenum type);

    ~Texture();

    /*
      INSTANCE METHODS
    */

    void Bind();

    void Unbind();


    /**
     * Enables texture tiling for this texture.
     */
    void SetTextureRepeat();

    /**
     * Enables texture clamping for this texture
     */
    void SetTextureClamping();

    void SetTextureWrap(const GLenum s, const GLenum t);
    void SetTextureWrap(const GLenum sAndT);


    void EnableAnisotropicFiltering();

    void SetBestMagMinFilters(const bool generateMipmap);

    void SetBestMagMinFilters();

    void GenerateMipmap();

    void SetMagMinFilters(const GLint filter);

    void SetMagFilter(const GLint filter);

    void SetMinFilter(const GLint filter);

    void ConfigureForPCF();

    /*
      Load the pixeldata of this texture from OpenGL and write it to a file.
     */
    void WriteToFile(const std::string& filename);

    void WriteDepthToFile(const std::string& filename);


    static void WriteToFile(unsigned char* pixels, const size_t width, const size_t height, const std::string& filename);

    /*
      String conversion operator.
     */
    operator std::string() const;


    /*
      GETTERS
    */

    GLsizei GetWidth() const;
    GLsizei GetHeight() const;
    GLenum GetTarget() const;
    GLuint GetHandle() const;

    /*
      STATIC METHODS:
     */

    static float GetMaxAnisotropic();

    static void SetActiveTextureUnit(const GLenum textureUnit);

    template<typename T>
    T* GetPixels(size_t size, GLenum format/* GL_RGBA*/, GLenum type/*GL_UNSIGNED_BYTE */) {
	// first, get the raw pixel data from opengl
	Bind();
//    size_t size = m_width* m_height * 4;
	T* pixels = new T[size];
	GL_C(glGetTexImage(m_target, 0, format, type, pixels));
	Unbind();
	return pixels;
    }


};
