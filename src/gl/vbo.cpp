#include "vbo.hpp"

#include "common.hpp"

VBO* VBO::createPosition(const GLint numComponents) {
    VBO* positionBuffer = new VBO();

    // configure VBO.
    positionBuffer->SetVertexAttribIndex(VBO_POSITION_ATTRIB_INDEX);
    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);
    positionBuffer->SetNumberOfVertexAttribComponents(numComponents);

    return positionBuffer;
}

VBO* VBO::createIndex(const GLenum type) {
    VBO* indexBuffer = new VBO();

    /**
     * Configure buffer.
     */
    indexBuffer->SetTarget(GL_ELEMENT_ARRAY_BUFFER);
    indexBuffer->SetType(type);
    indexBuffer->SetUsage(GL_STATIC_DRAW);

    return indexBuffer;
}

VBO* VBO::createTexCoord(const GLint numberOfComponents) {
    VBO* texcoordBuffer = new VBO();

    // configure buffer
    texcoordBuffer->SetVertexAttribIndex(VBO_TEX_COORD_ATTRIB_INDEX);
    texcoordBuffer->SetTarget(GL_ARRAY_BUFFER);
    texcoordBuffer->SetType(GL_FLOAT);
    texcoordBuffer->SetUsage(GL_STATIC_DRAW);
    texcoordBuffer->SetNumberOfVertexAttribComponents(numberOfComponents);

    return texcoordBuffer;
}

VBO* VBO::createNormal() {

    VBO* positionBuffer = new VBO();

    positionBuffer->SetVertexAttribIndex(VBO_NORMAL_ATTRIB_INDEX);
    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);
    positionBuffer->SetNumberOfVertexAttribComponents(3);

    return positionBuffer;
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
    SetBufferData(data.size()*sizeof(GL_UNSIGNED_SHORT), &data[0]);
}

void VBO::DrawIndices(const GLenum mode, const GLsizei count) {
    GL_C(glDrawElements(mode, count, m_type, 0));
}
