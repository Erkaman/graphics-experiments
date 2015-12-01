#include "fbo.hpp"


#include "log.hpp"

#include "gl/render_buffer.hpp"
#include "gl/texture.hpp"

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

    if(m_depthBuffer != NULL)
	delete m_depthBuffer;

    if(m_renderTargetTexture != NULL)

	delete m_renderTargetTexture;
}


void FBO::Attach(const  GLenum attachment, const Texture& texture) {
    GL_C(glFramebufferTexture2D(m_target, attachment, texture.GetTarget(), texture.GetHandle(), 0));
}

void FBO::CheckFramebufferStatus()  {
    CheckFramebufferStatus(m_target);
}


FBO::FBO(): m_target(GL_FRAMEBUFFER), m_depthBuffer(NULL), m_renderTargetTexture(NULL) {

}

void FBO::Init(const GLenum targetTextureUnit, const GLsizei width, const GLsizei height) {
    GL_C(glGenFramebuffers(1, &m_fboHandle));

    m_targetTextureUnit = targetTextureUnit;

    RecreateBuffers(width, height);
}

GLenum FBO::GetTargetTextureUnit()const {
    return m_targetTextureUnit;
}
