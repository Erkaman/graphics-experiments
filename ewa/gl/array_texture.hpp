#pragma once

#include "texture.hpp"

#include <map>

class TextureInfo;

class ArrayTexture : public Texture {

private:

    ArrayTexture(const std::vector<TextureInfo*>& tis, const std::vector<std::string>& textureFiles);


    static GLint GetMaxArrayTextureLayers();

    // maps every texture name to its corresponding w-value.
    // this w-value is used to access the texture in the array texture in a shader.
    std::map<std::string, GLint> textureMap;

public:

    static ArrayTexture* Load(const std::vector<std::string>& textureFiles);

    float GetTexture(const std::string& textureFile);
};
