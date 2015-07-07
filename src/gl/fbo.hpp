#pragma once

#include "render_buffer.hpp"

#include "texture.hpp"

class FBO{

private:

    FBO(const FBO&);
    FBO& operator=(const FBO&);

    GLuint m_fboHandle;
    const GLenum m_target;
    const GLenum m_targetTextureUnit;

    std::unique_ptr<RenderBuffer> m_depthBuffer;

    std::unique_ptr<Texture> m_renderTargetTexture;

    void Attach(const  GLenum attachment, const Texture& texture) {
	GL_C(glFramebufferTexture2D(m_target, attachment, texture.GetTarget(), texture.GetHandle(), 0));
    }


    static void CheckFramebufferStatus(const GLenum target);

    void CheckFramebufferStatus()  {
	CheckFramebufferStatus(m_target);
    }


public:

    FBO(const GLenum targetTextureUnit, const GLsizei width, const GLsizei height): m_target(GL_FRAMEBUFFER), m_targetTextureUnit(targetTextureUnit) {

	GL_C(glGenFramebuffers(1, &m_fboHandle));



	RecreateBuffers(width, height);
    }

    ~FBO()  {
	GL_C(glDeleteFramebuffers(1, &m_fboHandle));
    }


    /**
     * This method MUST be called every time the window is resized!
     */
    void RecreateBuffers(const GLsizei width, const GLsizei height);

    void BindForWriting() {
	GL_C(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboHandle));
    }

    void UnbindForWriting() {
	GL_C(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    }

    void Bind() {
	GL_C(glBindFramebuffer(GL_FRAMEBUFFER, m_fboHandle));
    }

    void Unbind() {
	GL_C(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

/*    void blitFrameBuffer() {
	glBlitFramebuffer(0, 0, Display.getWidth(), Display.getHeight(), 0, 0, Display.getWidth(), Display.getHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}*/

    void BindForReading() {
	GL_C(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboHandle));
    }

    void UnbindForReading() {
	GL_C(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    }

    Texture& GetRenderTargetTexture() const{
	return *m_renderTargetTexture;
    }

};
