#include "env_fbo.hpp"



void ColorFBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {

    width /= 2 * 2;
    height /= 2 * 2;

    Bind();
    {

	// first we create a render target, and attach it the FBO.
	m_renderTargetTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGBA8, // internal format
	    GL_RGBA, // format
	    GL_UNSIGNED_BYTE); // type

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_renderTargetTexture->Bind();
	{
	    m_renderTargetTexture->SetMagMinFilters(GL_LINEAR);
	    m_renderTargetTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT0, *m_renderTargetTexture);

	}
	m_renderTargetTexture->Unbind();

	// next we create a depth buffer, and attach it to the FBO.

	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorage(GL_DEPTH_COMPONENT, width, height);

	    // attach the depth buffer to the FBO.
	    glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle());
	}
	m_depthBuffer->Unbind();

	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();
}
