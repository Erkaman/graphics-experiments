#include "fbo.hpp"

#include "gl/texture.hpp"
#include "gl/render_buffer.hpp"

#include "log.hpp"


void FBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_renderTargetTexture = new Texture(GL_TEXTURE_2D, width, height, GL_RGBA8, GL_UNSIGNED_BYTE);

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

void FBO::CheckFramebufferStatus(const GLenum target)  {
    GLenum status;
    GL_C(status = glCheckFramebufferStatus(target));
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
	LOG_E("Framebuffer not complete. Status: %d", status);
    }
}

FBO::~FBO()  {
    GL_C(glDeleteFramebuffers(1, &m_fboHandle));

    delete m_depthBuffer;

    delete m_renderTargetTexture;
}


void FBO::Attach(const  GLenum attachment, const Texture& texture) {
    GL_C(glFramebufferTexture2D(m_target, attachment, texture.GetTarget(), texture.GetHandle(), 0));
}

void FBO::CheckFramebufferStatus()  {
    CheckFramebufferStatus(m_target);
}


FBO::FBO(const GLenum targetTextureUnit, const GLsizei width, const GLsizei height): m_target(GL_FRAMEBUFFER), m_targetTextureUnit(targetTextureUnit) {
    GL_C(glGenFramebuffers(1, &m_fboHandle));
    RecreateBuffers(width, height);
}

GLenum FBO::GetTargetTextureUnit() {
    return m_targetTextureUnit;
}
