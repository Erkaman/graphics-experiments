#pragma once

#include "texture.hpp"

#include <string>

class CubeMapTexture : public Texture{

private:


    bool LoadCubemapFace (const std::string face, int cubeMapTarget);


    CubeMapTexture();

public:

    static CubeMapTexture* Load(const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
		   const std::string& rightFace, const std::string& upFace, const std::string& downFace);


};
