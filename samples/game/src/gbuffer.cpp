#include "gbuffer.hpp"

#include "gl/texture.hpp"
#include "gl/render_buffer.hpp"

#include "log.hpp"

void Gbuffer::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// Color Texture
	m_colorTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGBA8, // internal format
	    GL_RGBA,  // format
	    GL_UNSIGNED_BYTE); // type

	Texture::SetActiveTextureUnit(COLOR_TEXTURE_UNIT);
	m_colorTexture->Bind();
	{
	    m_colorTexture->SetMagMinFilters(GL_NEAREST);
	    m_colorTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT0, *m_colorTexture);
	}
	m_colorTexture->Unbind();

	// Normal Texture:

	m_normalTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGBA16F, // internal format
	    GL_RGBA,  // format
	    GL_FLOAT); // type

	Texture::SetActiveTextureUnit(NORMAL_TEXTURE_UNIT);
	m_normalTexture->Bind();
	{
	    m_normalTexture->SetMagMinFilters(GL_NEAREST);
	    m_normalTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT1, *m_normalTexture);
	}
	m_normalTexture->Unbind();

	// Position:

	m_specularTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
/*
	    GL_RGBA8, // internal format
	    GL_RGBA,  // format
	    GL_UNSIGNED_BYTE); // type
*/

	    GL_RGBA16F, // internal format
	    GL_RGBA,  // format
	    GL_FLOAT); // type


	    // if position texture
/*
	    GL_RGBA32F, // internal format
	    GL_RGBA,  // format
	    GL_FLOAT); // type
*/
	Texture::SetActiveTextureUnit(SPECULAR_TEXTURE_UNIT);
	m_specularTexture->Bind();
	{
	    m_specularTexture->SetMagMinFilters(GL_NEAREST);
	    m_specularTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT2, *m_specularTexture);
	}
	m_specularTexture->Unbind();


	// first we create a render target, and attach it the FBO.
	m_depthTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_DEPTH_COMPONENT32, // internal format
	    GL_DEPTH_COMPONENT,  // format
	    GL_UNSIGNED_INT); // type

	Texture::SetActiveTextureUnit(DEPTH_TEXTURE_UNIT);
	m_depthTexture->Bind();
	{
	    m_depthTexture->SetMagMinFilters(GL_NEAREST);
	    m_depthTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_DEPTH_ATTACHMENT, *m_depthTexture);
	}
	m_depthTexture->Unbind();

//	GL_C(glReadBuffer(GL_NONE));
	GLenum tgts[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GL_C(glDrawBuffers(3, tgts));


	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();
}

Gbuffer::Gbuffer() {

}


Texture* Gbuffer::GetColorTexture() {
    return m_colorTexture;
}

Texture* Gbuffer::GetDepthTexture() {
    return m_depthTexture;
}

Texture* Gbuffer::GetNormalTexture() {
    return m_normalTexture;
}

Texture* Gbuffer::GetSpecularTexture() {
    return m_specularTexture;
}
