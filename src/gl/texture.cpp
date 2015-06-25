#include "texture.hpp"
#include "texture_loader.hpp"


GLfloat Texture::cachedMaxAnisotropic = 0.0f;

Texture::operator std::string() const {
        return "Texture ID: " + std::to_string(m_textureHandle) + ", " + "Width: "
	    + std::to_string(m_width) + ", " + "Height: "
	    + std::to_string(m_height) + ", ";
    }
