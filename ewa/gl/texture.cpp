#include "texture.hpp"
#include "math/math_common.hpp"

#include "lodepng.h"

#include "log.hpp"


GLfloat Texture::cachedMaxAnisotropic = 0.0f;

Texture::operator std::string() const {
    return "Texture ID: " + std::to_string(m_textureHandle) + ", " + "Width: "
	+ std::to_string(m_width) + ", " + "Height: "
	+ std::to_string(m_height) + ", ";
}



float Texture::GetMaxAnisotropic() {
    if(FloatEquals(cachedMaxAnisotropic, 0)) {
	GL_C(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &cachedMaxAnisotropic));
    }
    return cachedMaxAnisotropic;
}



Texture::Texture(const GLenum target, const GLsizei width, const GLsizei height, const GLint internalFormat,

		 const GLenum format,
		 const GLenum type):
    Texture(target) {

    Bind();

    m_width = width;
    m_height = height;

    GL_C(glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, type, NULL));

    Unbind();
}

void Texture::SetBestMagMinFilters(const bool generateMipmap) {
    if(generateMipmap)
	GenerateMipmap();

    EnableAnisotropicFiltering();

    GL_C(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_C(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
}

void Texture::SetMinFilter(const GLint filter) {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, filter ));
}


void Texture::SetMagFilter(const GLint filter) {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER,  filter));
}


void Texture::SetMagMinFilters(const GLint filter) {
    SetMagFilter(filter);
    SetMinFilter(filter);
}

void Texture::GenerateMipmap() {
    GL_C(glGenerateMipmap(m_target));
}

void Texture::SetActiveTextureUnit(const GLenum textureUnit) {
    GL_C(glActiveTexture(GL_TEXTURE0 + textureUnit));
}

void Texture::Bind() {
    GL_C(glBindTexture(m_target, m_textureHandle));
}


void Texture::Unbind() {
    GL_C(glBindTexture(m_target, 0));
}


void Texture::SetTextureRepeat() {
    SetTextureWrap(GL_REPEAT);
}

void Texture::SetTextureWrap(const GLenum s, const GLenum t) {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, s));
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, t));
}


void Texture::SetTextureWrap(const GLenum sAndT) {
    SetTextureWrap(sAndT, sAndT);
}

Texture::Texture(const GLenum target):m_target(target){
    GL_C(glGenTextures(1, &m_textureHandle));
}


Texture::~Texture()  {
    GL_C(glDeleteTextures(1, &m_textureHandle));
}


void Texture::SetTextureClamping() {
    SetTextureWrap(GL_CLAMP_TO_EDGE);
}


void Texture::EnableAnisotropicFiltering() {
    GL_C(glTexParameterf(m_target,GL_TEXTURE_MAX_ANISOTROPY_EXT, GetMaxAnisotropic()));
}

void Texture::SetBestMagMinFilters() {
    SetBestMagMinFilters(true);
}


GLsizei Texture::GetWidth() const{
    return m_width;
}

GLsizei Texture::GetHeight() const{
    return m_height;
}

GLenum Texture::GetTarget() const{
    return m_target;
}

GLuint Texture::GetHandle() const{
    return m_textureHandle;
}

void Texture::WriteToFile(const std::string& filename) {

    // first, get the raw pixel data from opengl
    Bind();
    size_t size = m_width* m_height * 4;
    unsigned char* pixels = new unsigned char[size];
    GL_C(glGetTexImage(m_target, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
    Unbind();

    // flip the image.

    unsigned int* intPixels = (unsigned int*)pixels;

    for (int i=0;i<m_width;++i){
	for (int j=0;j<m_height/2;++j){
	    unsigned int temp = intPixels[j * m_width + i];

	    intPixels[j * m_width + i] = intPixels[ (m_height-j-1)*m_width + i];
	    intPixels[(m_height-j-1)*m_width + i] = temp;
	}
    }

    unsigned char* charPixels = (unsigned char*)intPixels;

    WriteToFile(charPixels, m_width, m_height, filename);

    delete pixels;
}

void Texture::WriteDepthToFile(const std::string& filename) {

    // first, get the raw pixel data from opengl
    Bind();
    size_t size = m_width* m_height;
    float* depths = new float[size];
    GL_C(glGetTexImage(m_target, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depths));
    Unbind();

    // flip the image.
/*
    unsigned int* intPixels = (unsigned int*)pixels;

    for (int i=0;i<m_width;++i){
	for (int j=0;j<m_height/2;++j){
	    unsigned int temp = intPixels[j * m_width + i];

	    intPixels[j * m_width + i] = intPixels[ (m_height-j-1)*m_width + i];
	    intPixels[(m_height-j-1)*m_width + i] = temp;
	}
    }

    unsigned char* charPixels = (unsigned char*)intPixels;
*/


    unsigned char* rgbaPixels = new unsigned char[m_width* m_height * 4];
    int j = 0;

    for(size_t i = 0; i < size; ++i) {

	float depth = depths[i];

	char c = (unsigned char)(255.0f * depth);
	rgbaPixels[j++]  = c; // R
	rgbaPixels[j++]  = c; // G
	rgbaPixels[j++]  = c; // B
	rgbaPixels[j++]  = 255; // A


    }

    WriteToFile(rgbaPixels, m_width, m_height, filename);

    delete depths;
    delete rgbaPixels;



}


void Texture::WriteToFile(unsigned char* pixels, const size_t width, const size_t height, const std::string& filename) {

    //Encode the image
    unsigned error = lodepng::encode(filename, pixels, width, height);

  //if there's an error, display it
    if(error)
      LOG_E("PNG encoder error: %d: %s", error, lodepng_error_text(error) );
}

void Texture::ConfigureForPCF() {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    GL_C(glTexParameteri(m_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE));
}
