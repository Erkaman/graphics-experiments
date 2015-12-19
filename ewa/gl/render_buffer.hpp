#pragma once

#include "gl_common.hpp"

class RenderBuffer {
private:



    GLuint m_renderBufferHandle;
    const GLenum m_target;

    RenderBuffer(const RenderBuffer&);
    RenderBuffer& operator=(const RenderBuffer&);


public:

    RenderBuffer(): m_target(GL_RENDERBUFFER) {
	GL_C(glGenRenderbuffers(1, &m_renderBufferHandle) );
    }

    void Bind() {
	GL_C(glBindRenderbuffer(m_target, m_renderBufferHandle));
    }

    void Unbind() {
	GL_C(glBindRenderbuffer(m_target, 0));
    }

    void RenderbufferStorage(const GLenum internalformat, const GLsizei width, const GLsizei height) {
	GL_C(glRenderbufferStorage(m_target, internalformat, width, height));
    }

    void RenderbufferStorageMultisample(const GLsizei samples, const GLenum internalformat, const GLsizei width, const GLsizei height) {
	GL_C(glRenderbufferStorageMultisample(m_target, samples, internalformat, width, height));
    }

    GLuint GetHandle()const {
	return m_renderBufferHandle;
    }
};
