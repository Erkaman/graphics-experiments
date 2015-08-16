#pragma once

#include "gl_common.hpp"


class RenderBuffer;

class MSAA_FBO{

private:

    MSAA_FBO(const MSAA_FBO&);
    MSAA_FBO& operator=(const MSAA_FBO&);

    GLuint m_fboHandle;
    const GLenum m_target;

    RenderBuffer* m_depthBuffer;
    RenderBuffer* m_colorBuffer;

    static void CheckFramebufferStatus(const GLenum target);

    void CheckFramebufferStatus();


public:

    MSAA_FBO(const GLsizei width, const GLsizei height);

    ~MSAA_FBO();

    GLenum GetTargetTextureUnit();


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



};
