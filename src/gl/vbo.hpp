#pragma once

#include "gl_common.hpp"


class VBO {

private:

    GLuint m_buffer;
    GLuint m_vertexAttribIndex;
    GLenum m_target;
    GLenum m_type;
    GLint m_numberOfVertexAttribComponents;
    GLenum m_usage;

public:
    VBO() {
	GL_C(glGenBuffers(1, &m_buffer));
    }

    void Dispose() {
	GL_C(glDeleteBuffers(1, &m_buffer));
    }

    void SetVertexAttribIndex(GLuint vertexAttribIndex) {
        m_vertexAttribIndex = vertexAttribIndex;
    }

    void SetNumberOfVertexAttribComponents(GLint numberOfVertexAttribComponents);

    void SetTarget(GLenum target) {
        m_target = target;
    }

    void SetBufferData(GLsizeiptr size, const GLvoid* data);

    void EnableVertexAttrib();

    void DisableVertexAttrib();

    void Bind() {
        GL_C(glBindBuffer(m_target, m_buffer));
    }

    void Unbind() {
        GL_C(glBindBuffer(m_target, 0));
    }

    void SetType(GLenum type) {
        m_type = type;
    }

    void SetUsage(GLenum usage) {
        m_usage = usage;
    }
};

VBO* createPositionVBO(GLint numComponents);
VBO* createIndexVBO();
VBO* createTexCoordVBO(GLint numberOfComponents);
VBO* createNormalVBO();
