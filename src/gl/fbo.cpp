#include "fbo.hpp"

#include "common.hpp"

void FBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_renderTargetTexture = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, GL_RGBA8, GL_UNSIGNED_BYTE);

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

	m_depthBuffer = std::make_unique<RenderBuffer>();
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

void FBO::CheckFramebufferStatus(const GLenum target)  {
    GLenum status;
    GL_C(status = glCheckFramebufferStatus(target));
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
	LOG_E("Framebuffer not complete. Status: %d", status);
    }
}
