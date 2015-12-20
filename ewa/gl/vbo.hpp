#pragma once

#include "gl_common.hpp"
#include "mult_array.hpp"

#include <vector>

class Vector3f;
class Vector2f;

class VBO {

private:

    VBO(const VBO&);
    VBO& operator=(const VBO&);


    GLuint m_buffer;
    GLenum m_target;
    GLenum m_type;
    GLenum m_usage;

    bool m_isBound;

    std::vector<GLuint> m_sizes;
    std::vector<GLuint> m_offsets;
    GLsizei STRIDE;

public:
    VBO();

    ~VBO();

    void SetTarget(GLenum target);

    void SetBufferData(GLsizeiptr size, const GLvoid* data);

    void SetBufferData(const std::vector<Vector3f>& data);

    void SetBufferData(const std::vector<Vector2f>& data);
    void SetBufferData(const std::vector<GLushort>& data);
    void SetBufferData(const std::vector<GLuint>& data);
    void SetBufferData(const std::vector<GLfloat>& data);

    template<typename T>
    void SetBufferData(const MultArray<T>& m) {
	SetBufferData(m.GetTotalsize() * sizeof(T), reinterpret_cast<GLvoid *>(m.GetData()));
    }

    void EnableVertexAttribInterleaved();
    void DisableVertexAttribInterleaved();

    void Bind();
    void Unbind();

    void SetType(GLenum type);

    void SetUsage(GLenum usage);

    GLuint GetBuffer();


    void DrawIndices(const GLenum mode, const GLsizei count);
    void DrawVertices(const GLenum mode, const GLsizei count);

    void EnableVertexAttribInterleavedWithBind();
    void DisableVertexAttribInterleavedWithBind();


    static VBO* CreateIndex(const GLenum type = GL_UNSIGNED_SHORT);


    static VBO* CreateInterleaved(const std::vector<GLuint>& sizes, const GLenum usage = GL_STATIC_DRAW);

    static void DrawIndices(VBO& vertexBuffer, VBO& indexBuffer, const GLenum mode, const GLsizei count);

    static void DrawVertices(VBO& vertexBuffer, const GLenum mode, const GLsizei count);


    void GetBufferSubData(GLintptr offset, GLsizeiptr size, GLvoid* data);

    GLint GetBufferSize();

};
