#include "vbo.hpp"

#include "math/vector3f.hpp"
#include "math/vector2f.hpp"

#include "ewa/log.hpp"

#include <assert.h>


VBO::VBO():m_isBound(false) {
    GL_C(glGenBuffers(1, &m_buffer));
}

VBO::~VBO() {
    GL_C(glDeleteBuffers(1, &m_buffer));
}


VBO* VBO::CreateIndex(const GLenum type) {
    VBO* indexBuffer = new VBO();

    indexBuffer->SetTarget(GL_ELEMENT_ARRAY_BUFFER);
    indexBuffer->SetType(type);
    indexBuffer->SetUsage(GL_STATIC_DRAW);

    return indexBuffer;
}

VBO* VBO::CreateInterleaved(const std::vector<GLuint>& sizes, const GLenum usage){

    VBO* buffer = new VBO();

    buffer->SetTarget(GL_ARRAY_BUFFER);
    buffer->SetType(GL_FLOAT);
    buffer->SetUsage(usage);

    buffer->m_sizes = sizes;

    buffer->m_offsets.reserve(sizes.size());

    GLuint totalOffset = 0;
    for(size_t i = 0; i <buffer->m_sizes.size(); ++i) {

	buffer->m_offsets.push_back(totalOffset);
        totalOffset += buffer->m_sizes[i] * sizeof(GLfloat);


    }
    buffer->STRIDE = totalOffset;

    return buffer;
}

void VBO::SetBufferData(GLsizeiptr size, const GLvoid* data) {
    GL_C(glBufferData(m_target,size, data, m_usage));
}


void VBO::SetBufferData(const std::vector<Vector3f>& data) {
    SetBufferData(data.size()*sizeof(float)*3, &data[0]);
}

void VBO::SetBufferData(const std::vector<Vector2f>& data) {
    SetBufferData(data.size()*sizeof(float)*2, &data[0]);
}

void VBO::SetBufferData(const std::vector<GLushort>& data) {
    SetBufferData(data.size()*sizeof(GLushort), &data[0]);
}

void VBO::SetBufferData(const std::vector<GLuint>& data) {
    SetBufferData(data.size()*sizeof(GLuint), &data[0]);
}


void VBO::SetBufferData(const std::vector<GLfloat>& data) {
    SetBufferData(data.size()*sizeof(GLfloat), reinterpret_cast<const GLvoid*>(&data[0]));
}

void VBO::DrawIndices(const GLenum mode, const GLsizei count) {
    GL_C(glDrawElements(mode, count, m_type, 0));
}

void VBO::DrawVertices(const GLenum mode, const GLsizei count) {
    GL_C(glDrawArrays(mode, 0, count));
}

void VBO::EnableVertexAttribInterleaved() {

    Bind();

    GLuint vertexAttrib = 0;

    for(size_t i = 0; i < m_sizes.size(); ++i) {


	const GLuint size = m_sizes[i];
	const GLuint offset = m_offsets[i];


	GL_C(glEnableVertexAttribArray(vertexAttrib));
	GL_C(glVertexAttribPointer(vertexAttrib, size, GL_FLOAT, false, STRIDE, reinterpret_cast<const GLvoid*>(offset)));

	++vertexAttrib;
    }

    Unbind();
}

void VBO::DisableVertexAttribInterleaved() {
    Bind();

    GLuint vertexAttrib = 0;

    for(size_t i = 0; i < m_sizes.size(); ++i) {

	GL_C(glDisableVertexAttribArray(vertexAttrib));

	++vertexAttrib;
    }

    Unbind();
}

void VBO::EnableVertexAttribInterleavedWithBind() {
    Bind();
    EnableVertexAttribInterleaved();
    Unbind();
}


void VBO::DisableVertexAttribInterleavedWithBind() {
    Bind();
    DisableVertexAttribInterleaved();
    Unbind();
}


void VBO::DrawIndices(VBO& vertexBuffer, VBO& indexBuffer, const GLenum mode, const GLsizei count) {

    vertexBuffer.EnableVertexAttribInterleavedWithBind();

    indexBuffer.Bind();

    indexBuffer.DrawIndices(mode, count);

    indexBuffer.Unbind();

    vertexBuffer.DisableVertexAttribInterleavedWithBind();
}

void VBO::DrawVertices(VBO& vertexBuffer, const GLenum mode, const GLsizei count) {

    vertexBuffer.EnableVertexAttribInterleavedWithBind();

    vertexBuffer.DrawVertices(mode, count);

    vertexBuffer.DisableVertexAttribInterleavedWithBind();
}

void VBO::SetTarget(GLenum target) {
    m_target = target;
}

void VBO::SetType(GLenum type) {
    m_type = type;
}
void VBO::SetUsage(GLenum usage) {
    m_usage = usage;
}

void VBO::Bind() {
    GL_C(glBindBuffer(m_target, m_buffer));
}

void VBO::Unbind() {
    GL_C(glBindBuffer(m_target, 0));
}

GLuint VBO::GetBuffer() {
    return m_buffer;
}


void VBO::GetBufferSubData(GLintptr offset, GLsizeiptr size, GLvoid* data) {
    GL_C(glGetBufferSubData(m_target, offset, size, data));
}


GLint VBO::GetBufferSize() {
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    return bufferSize;
}
