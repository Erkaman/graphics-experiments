#include "cube_map_texture.hpp"

#include "texture_loader.hpp"

CubeMapTexture::CubeMapTexture(const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
			       const std::string& rightFace, const std::string& upFace, const std::string& downFace):
    Texture(GL_TEXTURE_CUBE_MAP){
    Texture::SetActiveTextureUnit(0);
    Bind();
    LoadCubemapFace(leftFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    LoadCubemapFace(rightFace, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    LoadCubemapFace(downFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    LoadCubemapFace(upFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    LoadCubemapFace(backFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    LoadCubemapFace(frontFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    GenerateMipmap();
    Unbind();
}

void CubeMapTexture::LoadCubemapFace (const std::string face, int cubeMapTarget) {

    TextureInfo ti(TextureLoader::Load(face));
			// upload texture to OpenGL

    glTexImage2D(cubeMapTarget, 0, ti.internalFormat , ti.width, ti.height, 0, ti.format, ti.type, &ti.imageData[0]);
    m_width = ti.width;
    m_height = ti.height;
}


/*
  Texture2D::Texture2D(const std::string& texturePath): Texture(GL_TEXTURE_2D) {
  TextureInfo ti = TextureLoader::Load(texturePath);

  Bind();
  {
  GL_C(glTexImage2D(m_target, 0, ti.internalFormat , ti.width, ti.height, 0, ti.format, ti.type, &ti.imageData[0]));
  m_width = ti.width;
  m_height = ti.height;
  }
  Unbind();

  }
*/
