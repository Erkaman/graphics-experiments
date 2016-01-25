#include "array_texture.hpp"

#include "texture_loader.hpp"
#include "log.hpp"

using std::vector;
using std::string;

ArrayTexture* ArrayTexture::Load(const vector<string>& textureFiles) {

    vector<TextureInfo*> tis;

    for(const string& textureFile : textureFiles) {

	TextureInfo* ti = TextureLoader::Load(textureFile);

	if(!ti) {
	    return NULL;
	}


	tis.push_back(ti);
    }

    ArrayTexture* texture = new ArrayTexture(tis, textureFiles);

    return texture;

}

ArrayTexture::ArrayTexture(const vector<TextureInfo*>& tis, const vector<string>& textureFiles): Texture(GL_TEXTURE_2D_ARRAY) {

    if(GetMaxArrayTextureLayers() < tis.size()) {


	LOG_E(
	    "%ld are too many layers for an array texture on this machine. On this machine, the maximum number is %d",
	    tis.size(),
	    GetMaxArrayTextureLayers()
	    );
    }


    Bind();
    {
	GLsizei numLayers = tis.size();
//	GLsizei numLayers = buffers.size();

	// do note that the parameters of the first image is used for ALL the other
	// textures in the texture array!
	TextureInfo* first = tis[0];

	GLint internalFormat = first->internalFormat;
	GLenum format = first->format;
	GLsizei width = first->width;
	GLsizei height = first->height;
	GLenum type = first->type;

	GL_C(glTexImage3D(m_target, 0,
			  internalFormat,
			  width,
			  height,
			  numLayers,
			  0,
			  format,
			  type,
			  nullptr
		 ));

	// upload the 2D images into the 3D texture.
	for(int iLayer = 0; iLayer < tis.size(); ++iLayer) {

	    GL_C(glTexSubImage3D(m_target,
				 0,
				 0, 0, iLayer,
				 width,
				 height,
				 1, // depth
				 format,
				 type,
				 &tis[iLayer]->imageData[0]
		     ));

	    textureMap[textureFiles[iLayer]] = iLayer;

	    LOG_I("bla: %s = %d", textureFiles[iLayer].c_str(), iLayer );

	}


	Unbind();


    }
}


GLint ArrayTexture::GetMaxArrayTextureLayers() {
    GLint res;
    GL_C(glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &res));
    return res;
}

float ArrayTexture::GetTexture(const std::string& textureFile) {
    return (float)textureMap[textureFile];
}
