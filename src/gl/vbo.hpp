#pragma once

#include "gl_common.hpp"

#include <vector>

class Vector3f;
class Vector2f;

class VBO {

private:

    GLuint m_buffer;
    GLuint m_vertexAttribIndex;
    GLenum m_target;
    GLenum m_type;
    GLint m_numberOfVertexAttribComponents;
    GLenum m_usage;

    // only valid for interleaved VBOs:
    std::vector<GLuint> m_vertexAttribs;
    std::vector<GLuint> m_sizes;
    std::vector<GLuint> m_offsets;
    GLsizei STRIDE;

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

    void SetBufferData(const std::vector<Vector3f>& data);

    void SetBufferData(const std::vector<Vector2f>& data);
    void SetBufferData(const std::vector<GLushort>& data);
    void SetBufferData(const std::vector<GLfloat>& data);

    void EnableVertexAttrib();
    void DisableVertexAttrib();

    void EnableVertexAttribInterleaved();
    void DisableVertexAttribInterleaved();


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

    void DrawIndices(const GLenum mode, const GLsizei count);

    static VBO* CreatePosition(const GLint numComponents);
    static VBO* CreateIndex(const GLenum type = GL_UNSIGNED_SHORT);
    static VBO* CreateTexCoord(const GLint numberOfComponents);
    static VBO* CreateNormal();

    static VBO* CreateInterleaved(std::vector<GLuint>&& vertexAttribs, std::vector<GLuint>&& sizes);


};

// the data needed,
// the vertex attribs(vector of GLuint)
// and the sizes.
