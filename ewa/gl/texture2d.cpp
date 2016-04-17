#include "texture2d.hpp"

#include "texture_loader.hpp"

#include "ewa/common.hpp"


 Texture2D* Texture2D::Load(const std::string& texturePath) {


     if(texturePath.substr(texturePath.size()-4, 4 ) == ".png" ) {

	 TextureInfo* ti = TextureLoader::Load(texturePath);

	 if(!ti) {
	     return NULL;
	 }

	 Texture2D* texture = new Texture2D(
	     &ti->imageData[0],
	     ti->width,
	     ti->height,
	     ti->internalFormat,
	     ti->format,
	     ti->type
	     );

	 return texture;
     } else {

	 DdsInfo* di = TextureLoader::LoadDds(texturePath);

	 if(!di) {
	     return NULL;
	 }

	 Texture2D* texture = new Texture2D(
	     di
	     );

	 return texture;
     }

}

Texture2D::Texture2D(DdsInfo* di): Texture(GL_TEXTURE_2D) {

    LOG_E("WE SHOULD NOT GET HERE!");

/*
    unsigned int blockSize = (di->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;


 unsigned int offset = 0;

 Bind();

 for (unsigned int level = 0; level < di->mipMapCount && (di->width || di->height); ++level)
     {
         unsigned int size = ((di->width+3)/4)*((di->height+3)/4)*blockSize;
         glCompressedTexImage2D(GL_TEXTURE_2D, level, di->format, di->width, di->height,
             0, size, di->buffer + offset);

         offset += size;
         di->width  /= 2;
         di->height /= 2;
     }

 Unbind();
 MY_DELETE(di->buffer);
*/


}

Texture2D::Texture2D(GLvoid* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum type): Texture(GL_TEXTURE_2D) {

    m_level = 0;
    m_format = format;
    m_type = type;

    m_width = width;
    m_height = height;

     Bind();
    {
	GL_C(glTexImage2D(m_target, m_level,
			  internalFormat, // internal format.
			  m_width, m_height, 0, m_format, m_type, data));
    }
    Unbind();
}

void Texture2D::UpdateTexture(GLvoid* data) {
    GL_C(glTexSubImage2D(
	     m_target,
	     m_level,
	     0, //xoffset
	     0, //yoffset
	     m_width,
	     m_height,
	     m_format,
	     m_type,
	     data));
}
