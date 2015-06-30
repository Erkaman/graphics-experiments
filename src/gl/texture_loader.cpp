#include "texture_loader.hpp"

#include "lodepng.h"

#include "common.hpp"

#include <iostream>

using std::vector;

TextureInfo TextureLoader::Load(const std::string& texturePath) {
    TextureInfo textureInfo;

    unsigned int width;
    unsigned int height;

    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, texturePath);

    lodepng::State state;
    unsigned error = lodepng::decode(textureInfo.imageData, width, height, state, buffer);

    if(error != 0){
	LOG_E("could not load png %s: %s", texturePath.c_str(), lodepng_error_text(error));
    }

/*    const LodePNGInfo info = state.info_png;
    const LodePNGColorMode& color = info.color;
*/

/*    if(! (LCT_RGBA == color.colortype &&
	  32 == lodepng_get_bpp(&color) // bits per pixel.
	   ) ) {
	LOG_E("unsupported png format");
    }*/

    vector<unsigned char>& imageData = textureInfo.imageData;

    for(size_t i = 0; i < imageData.size(); i += 4) {
	unsigned char r = imageData[i+0];
	unsigned char g = imageData[i+1];
	unsigned char b = imageData[i+2];

	imageData[i+0] = b;
	imageData[i+1] = g;
	imageData[i+2] = r;
    }

    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.format =  GL_BGRA;
    textureInfo.internalFormat = GL_RGBA8;
    textureInfo.type =  GL_UNSIGNED_INT_8_8_8_8_REV;

    return textureInfo;
}
