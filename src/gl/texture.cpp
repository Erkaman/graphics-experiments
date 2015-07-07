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
