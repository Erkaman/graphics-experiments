#include "gbuffer.hpp"

#include "gl/texture.hpp"
#include "gl/render_buffer.hpp"

#include "log.hpp"

void Gbuffer::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_colorTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGBA8, // internal format
	    GL_RGBA,  // format
	    GL_UNSIGNED_BYTE); // type

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_colorTexture->Bind();
	{
	    m_colorTexture->SetMagMinFilters(GL_NEAREST);
	    m_colorTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT0, *m_colorTexture);
	}

	m_colorTexture->Unbind();

	// next we create a depth buffer, and attach it to the FBO.

	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorage(GL_DEPTH_COMPONENT, width, height);

	    // attach the depth buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle()));
	}
	m_depthBuffer->Unbind();


	/*GL_C(glReadBuffer(GL_NONE));
	GL_C(glDrawBuffer(GL_COLOR_ATTACHMENT0));
	*/

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
