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
	LOG_E("could not load png: %s", lodepng_error_text(error));
    }

    const LodePNGInfo info = state.info_png;
    const LodePNGColorMode& color = info.color;


/*    if(! (LCT_RGBA == color.colortype &&
	  32 == lodepng_get_bpp(&color) // bits per pixel.
	   ) ) {
	LOG_E("unsupported png format");
    }*/

    vector<unsigned char>& imageData = textureInfo.imageData;

    for(int i = 0; i < imageData.size(); i += 4) {
	unsigned char r = imageData[i+0];
	unsigned char g = imageData[i+1];
	unsigned char b = imageData[i+2];
	unsigned char a = imageData[i+3];

	imageData[i+0] = 255;
	imageData[i+1] = 255;
	imageData[i+2] = 255;
	imageData[i+3] = 255;

    }

    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.format =  GL_RGBA; //GL_BGRA;
    textureInfo.internalFormat = GL_RGBA; //GL_RGBA8;
    textureInfo.type =  GL_UNSIGNED_BYTE; //GL_UNSIGNED_INT_8_8_8_8_REV;

    LOG_I("internal format: %d", textureInfo.internalFormat);

    return textureInfo;
}
