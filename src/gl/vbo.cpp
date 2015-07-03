#include "vbo.hpp"

#include "common.hpp"

#include "math/vector3f.hpp"
#include "math/vector2f.hpp"

#include <assert.h>


VBO* VBO::CreatePosition(const GLint numComponents) {
    VBO* positionBuffer = new VBO();

    // configure VBO.
    positionBuffer->SetVertexAttribIndex(VBO_POSITION_ATTRIB_INDEX);
    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);
    positionBuffer->SetNumberOfVertexAttribComponents(numComponents);

    return positionBuffer;
}

VBO* VBO::CreateIndex(const GLenum type) {
    VBO* indexBuffer = new VBO();

    /**
     * Configure buffer.
     */
    indexBuffer->SetTarget(GL_ELEMENT_ARRAY_BUFFER);
    indexBuffer->SetType(type);
    indexBuffer->SetUsage(GL_STATIC_DRAW);

    return indexBuffer;
}

VBO* VBO::CreateTexCoord(const GLint numberOfComponents) {
    VBO* texcoordBuffer = new VBO();

    // configure buffer
    texcoordBuffer->SetVertexAttribIndex(VBO_TEX_COORD_ATTRIB_INDEX);
    texcoordBuffer->SetTarget(GL_ARRAY_BUFFER);
    texcoordBuffer->SetType(GL_FLOAT);
    texcoordBuffer->SetUsage(GL_STATIC_DRAW);
    texcoordBuffer->SetNumberOfVertexAttribComponents(numberOfComponents);

    return texcoordBuffer;
}

VBO* VBO::CreateNormal() {

    VBO* positionBuffer = new VBO();

    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);

    return positionBuffer;
}

VBO* VBO::CreateInterleaved(const std::vector<GLuint>&& vertexAttribs, const std::vector<GLuint>&& sizes, const GLenum usage){

    assert(vertexAttribs.size() == sizes.size());

    VBO* buffer = new VBO();

    buffer->SetVertexAttribIndex(VBO_NORMAL_ATTRIB_INDEX);
    buffer->SetTarget(GL_ARRAY_BUFFER);
    buffer->SetType(GL_FLOAT);
    buffer->SetUsage(usage);

    buffer->m_vertexAttribs = std::move(vertexAttribs);
    buffer->m_sizes = std::move(sizes);

    buffer->m_offsets.reserve(sizes.size());

    GLuint totalOffset = 0;
    for(size_t i = 0; i <buffer->m_sizes.size(); ++i) {
	buffer->m_offsets.push_back(totalOffset);
        totalOffset += buffer->m_sizes[i] * sizeof(GLfloat);
    }
    buffer->STRIDE = totalOffset;

    return buffer;
}

void VBO::EnableVertexAttrib() {
    GL_C(glEnableVertexAttribArray(m_vertexAttribIndex));
    GL_C(glVertexAttribPointer(m_vertexAttribIndex, m_numberOfVertexAttribComponents, m_type, false, 0, 0));
}

void VBO::DisableVertexAttrib() {
    GL_C(glDisableVertexAttribArray(m_vertexAttribIndex));
}

void VBO::SetNumberOfVertexAttribComponents(GLint numberOfVertexAttribComponents) {
    m_numberOfVertexAttribComponents = numberOfVertexAttribComponents;
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

    for(size_t i = 0; i < m_sizes.size(); ++i) {

	const GLuint vertexAttrib = m_vertexAttribs[i];
	const GLuint size = m_sizes[i];
	const GLuint offset = m_offsets[i];

	GL_C(glEnableVertexAttribArray(vertexAttrib));
	GL_C(glVertexAttribPointer(vertexAttrib, size, GL_FLOAT, false, STRIDE, reinterpret_cast<const GLvoid*>(offset)));
    }

    Unbind();
}

void VBO::DisableVertexAttribInterleaved() {
    Bind();

    for(size_t i = 0; i < m_sizes.size(); ++i) {
	const GLuint vertexAttrib = m_vertexAttribs[i];

	GL_C(glDisableVertexAttribArray(vertexAttrib));
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

void VBO::EnableVertexAttribWithBind() {
    Bind();
    EnableVertexAttrib();
    Unbind();
}

void VBO::DisableVertexAttribWithBind() {
    Bind();
    DisableVertexAttrib();
    Unbind();
}


void VBO::DrawIndices(VBO& vertexBuffer, VBO& indexBuffer, const GLenum mode, const GLsizei count) {

    vertexBuffer.EnableVertexAttribInterleavedWithBind();

    indexBuffer.Bind();

    indexBuffer.DrawIndices(mode, count);

    indexBuffer.Unbind();

    vertexBuffer.DisableVertexAttribInterleavedWithBind();
}
