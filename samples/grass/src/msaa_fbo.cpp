#include "msaa_fbo.hpp"

#include "gl/render_buffer.hpp"
#include "gl/fbo.hpp"
#include "gl/texture.hpp"

#include "log.hpp"

void MSAA_FBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {

    constexpr int SAMPLES = 0;

    Bind();
    {

	m_colorBuffer = new RenderBuffer();
	m_colorBuffer->Bind();
	{
	    m_colorBuffer->RenderbufferStorageMultisample(SAMPLES, GL_RGBA8, width, height);

	    // attach the color buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,m_colorBuffer->GetHandle()));
	}
	m_colorBuffer->Unbind();

	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorageMultisample(SAMPLES, GL_DEPTH_COMPONENT, width, height);

	    // attach the depth buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle()));
	}
	m_depthBuffer->Unbind();


	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();


}

MSAA_FBO::MSAA_FBO(const GLsizei width, const GLsizei height): m_target(GL_FRAMEBUFFER), m_width(width), m_height(height){
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

void MSAA_FBO::WriteToFile(const std::string& filename) {

    FBO fbo(9, m_width, m_height);

    this->BindForReading();
    fbo.BindForWriting();

    GL_C(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

     this->UnbindForReading();
     fbo.UnbindForWriting();

     fbo.GetRenderTargetTexture().WriteToFile(filename);
}
