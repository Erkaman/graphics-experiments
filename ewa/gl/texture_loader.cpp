#include "texture_loader.hpp"

#include "log.hpp"

#include "lodepng.h"

#include "resource_manager.hpp"


using std::vector;

TextureInfo* TextureLoader::Load(const std::string& texturePath) {
    TextureInfo* textureInfo = new TextureInfo();

    unsigned int width;
    unsigned int height;

    std::string* resourcePath = ResourceManager::GetInstance().SearchResource(texturePath);



    if(!resourcePath) {
	return NULL;
    }


    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer,
		       *resourcePath);

    lodepng::State state;
    unsigned error = lodepng::decode(textureInfo->imageData, width, height, state, buffer);

    if(error != 0){
	LOG_I("ERROR");
	SetError("could not load png %s: %s", resourcePath->c_str(), lodepng_error_text(error));
	return NULL;
    }

    vector<unsigned char>& imageData = textureInfo->imageData;

    for(size_t i = 0; i < imageData.size(); i += 4) {
	unsigned char r = imageData[i+0];
	unsigned char g = imageData[i+1];
	unsigned char b = imageData[i+2];

	imageData[i+0] = b;
	imageData[i+1] = g;
	imageData[i+2] = r;
    }

    textureInfo->width = width;
    textureInfo->height = height;
    textureInfo->format =  GL_BGRA;
    textureInfo->internalFormat = GL_RGBA8;
    textureInfo->type =  GL_UNSIGNED_INT_8_8_8_8_REV;

    return textureInfo;
}

DdsInfo* TextureLoader::LoadDds(const std::string& texturePath) {
#if 0

    File* f = File::Load(texturePath, FileModeReading);

    if(!f) {
	return NULL;
    }

    char magic[4];
    f->ReadArray(magic, 4 * sizeof(char));

    if (strncmp(magic, "DDS ", 4) != 0) {
	SetError("%s is not a DDS file: invalid magic number %d", texturePath.c_str(), *(int *)&magic[0] );
	return NULL;
    }

    unsigned char header[124];

    f->ReadArray(header, 124 * sizeof(unsigned char));

    DdsInfo* ddsInfo = new DdsInfo();

    ddsInfo->height      = *(unsigned int*)&(header[8 ]);
    ddsInfo->width         = *(unsigned int*)&(header[12]);
    unsigned int linearSize     = *(unsigned int*)&(header[16]);
    ddsInfo->mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = ddsInfo->mipMapCount > 1 ? linearSize * 2 : linearSize;
    ddsInfo->buffer = new unsigned char[bufsize];

    /*
    LOG_I("count: %d",ddsInfo->mipMapCount );
    LOG_I("height: %d",ddsInfo->height );
    LOG_I("width: %d",ddsInfo->width );

    LOG_I("bufsize: %d", bufsize );


    LOG_I("dat: %c", header[0] );
    LOG_I("dat: %c", header[1] );
    LOG_I("dat: %c", header[2] );
    LOG_I("dat: %c", header[3] );
    */


    f->ReadArray(ddsInfo->buffer, bufsize *  sizeof(unsigned char));

//     fread(buffer, 1, bufsize, fp);
    /* close the file pointer */

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII


    ddsInfo->components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
    case FOURCC_DXT1:
	ddsInfo->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	break;
    case FOURCC_DXT3:
	ddsInfo->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
	ddsInfo->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	break;
    default:
	free(ddsInfo->buffer);
	return 0;
    }

    return ddsInfo;

#endif
}
