#include "cube_map_texture.hpp"

#include "texture_loader.hpp"


CubeMapTexture::CubeMapTexture():Texture(GL_TEXTURE_CUBE_MAP) {

}

CubeMapTexture* CubeMapTexture::Load(
    const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
    const std::string& rightFace, const std::string& upFace, const std::string& downFace){

    CubeMapTexture* cubeMap = new CubeMapTexture();

    Texture::SetActiveTextureUnit(0);
    cubeMap->Bind();

    if(!cubeMap->LoadCubemapFace(leftFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_X)) return NULL;
    if(!cubeMap->LoadCubemapFace(rightFace, GL_TEXTURE_CUBE_MAP_POSITIVE_X))return NULL;
    if(!cubeMap->LoadCubemapFace(downFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y))return NULL;
    if(!cubeMap->LoadCubemapFace(upFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Y))return NULL;
    if(!cubeMap->LoadCubemapFace(backFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))return NULL;
    if(!cubeMap->LoadCubemapFace(frontFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Z))return NULL;
    cubeMap->GenerateMipmap();
    cubeMap->Unbind();

    return cubeMap;
}

bool CubeMapTexture::LoadCubemapFace (const std::string face, int cubeMapTarget) {

    TextureInfo* ti = TextureLoader::Load(face);
    // upload texture to OpenGL

    if(!ti) {
	return false;
    }

    glTexImage2D(cubeMapTarget, 0, ti->internalFormat , ti->width, ti->height, 0, ti->format, ti->type, &ti->imageData[0]);
    m_width = ti->width;
    m_height = ti->height;

    return true;
}
