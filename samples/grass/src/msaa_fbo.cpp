#include "msaa_fbo.hpp"

#include "gl/render_buffer.hpp"
#include "log.hpp"

void MSAA_FBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {

	m_colorBuffer = new RenderBuffer();
	m_colorBuffer->Bind();
	{
	    m_colorBuffer->RenderbufferStorageMultisample(4, GL_RGBA8, width, height);

	    // attach the color buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,m_colorBuffer->GetHandle()));
	}
	m_colorBuffer->Unbind();






	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorageMultisample(4, GL_DEPTH_COMPONENT, width, height);

	    // attach the depth buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle()));
	}
	m_depthBuffer->Unbind();


	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();


}

MSAA_FBO::MSAA_FBO(const GLsizei width, const GLsizei height): m_target(GL_FRAMEBUFFER){
    GL_C(glGenFramebuffers(1, &m_fboHandle));
    RecreateBuffers(width, height);
}

void MSAA_FBO::CheckFramebufferStatus(const GLenum target)  {
    GLenum status;
    GL_C(status = glCheckFramebufferStatus(target));
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
	LOG_E("Framebuffer not complete. Status: %d", status);
    }
}

void MSAA_FBO::CheckFramebufferStatus()  {
    CheckFramebufferStatus(m_target);
}
