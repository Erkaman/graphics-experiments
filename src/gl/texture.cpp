#include "texture.hpp"
#include "math/math_common.hpp"


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



Texture::Texture(const GLenum target, const GLsizei width, const GLsizei height, const GLint internalFormat, const GLenum type):
    Texture(target) {

    Bind();

    m_width = width;
    m_height = height;

    GL_C(glTexImage2D(m_target, 0, internalFormat, width, height, 0, GL_RGBA, type, NULL));

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
    if(m_alreadyBound)
	return;

    GL_C(glBindTexture(m_target, m_textureHandle));
    m_alreadyBound = true;
}


void Texture::Unbind() {
    if(!m_alreadyBound)
	return;

    GL_C(glBindTexture(m_target, 0));
    m_alreadyBound = false;
}


void Texture::SetTextureTiling() {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT));
}


Texture::Texture(const GLenum target):m_alreadyBound(false), m_target(target){
    GL_C(glGenTextures(1, &m_textureHandle));
}


Texture::~Texture()  {
    GL_C(glDeleteTextures(1, &m_textureHandle));
}


void Texture::SetTextureClamping() {
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_C(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
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
