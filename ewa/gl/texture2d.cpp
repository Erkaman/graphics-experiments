#include "texture2d.hpp"

#include "texture_loader.hpp"


 Texture2D* Texture2D::Load(const std::string& texturePath) {

    TextureInfo* ti = TextureLoader::Load(texturePath);

    if(!ti) {
	return NULL;
    }

    Texture2D* texture = new Texture2D(
	&ti->imageData[0],
	ti->width,
	ti->height,
	ti->internalFormat,
	ti->format,
	ti->type
	);

    return texture;

}


/*
Texture2D::Texture2D(const std::string& texturePath): Texture(GL_TEXTURE_2D) {

    Bind();
    {
	GL_C(glTexImage2D(m_target, 0, ti.internalFormat , ti.width, ti.height, 0, ti.format, ti.type, &ti.imageData[0]));
	m_width = ti.width;
	m_height = ti.height;
    }
    Unbind();

}
*/

Texture2D::Texture2D(GLvoid* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum type): Texture(GL_TEXTURE_2D) {
     Bind();
    {
	GL_C(glTexImage2D(m_target, 0,
			  internalFormat, // internal format.
			  width, height, 0, format, type, data));
	m_width = width;
	m_height = width;
    }
    Unbind();
}
