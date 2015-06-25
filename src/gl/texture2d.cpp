#include "texture2d.hpp"

#include "texture_loader.hpp"

#include "log.hpp"

Texture2D::Texture2D(const std::string& texturePath): Texture(GL_TEXTURE_2D) {
    TextureInfo ti = TextureLoader::Load(texturePath);

    LOG_I("target: %d", GL_TEXTURE_2D);

    Bind();
    {

	LOG_I("actual target: %d", m_target);
	LOG_I("actual width: %d", ti.width);
	LOG_I("actual height: %d", ti.height);
	LOG_I("actual internal format: %d", ti.internalFormat);

	LOG_I("data: %d", ti.imageData[0]);
	LOG_I("data: %d", ti.imageData[1]);
	LOG_I("data: %d", ti.imageData[2]);
	LOG_I("data: %d", ti.imageData[3]);
	LOG_I("data: %d", ti.imageData[4]);
	LOG_I("data: %d", ti.imageData[5]);
	LOG_I("data: %d", ti.imageData[6]);
	LOG_I("data: %d", ti.imageData[7]);

	GL_C(glTexImage2D(m_target, 0, ti.internalFormat , ti.width, ti.height, 0, ti.format, ti.type, &ti.imageData[0]));
	m_width = ti.width;
	m_height = ti.height;
    }
    Unbind();

}
