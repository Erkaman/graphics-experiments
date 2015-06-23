#include "vbo.hpp"

#include "common.hpp"

VBO* createPositionVBO(GLint numComponents) {
    VBO* positionBuffer = new VBO();

    // configure VBO.
    positionBuffer->SetVertexAttribIndex(VBO_POSITION_ATTRIB_INDEX);
    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);
    positionBuffer->SetNumberOfVertexAttribComponents(numComponents);

    return positionBuffer;
}

VBO* createIndexVBO() {
    VBO* indexBuffer = new VBO();

    /**
     * Configure buffer.
     */
    indexBuffer->SetTarget(GL_ELEMENT_ARRAY_BUFFER);
    indexBuffer->SetType(GL_INT);
    indexBuffer->SetUsage(GL_STATIC_DRAW);

    return indexBuffer;
}

VBO* createTexCoordVBO(GLint numberOfComponents) {
    VBO* texcoordBuffer = new VBO();

    // configure buffer
    texcoordBuffer->SetVertexAttribIndex(VBO_TEX_COORD_ATTRIB_INDEX);
    texcoordBuffer->SetTarget(GL_ARRAY_BUFFER);
    texcoordBuffer->SetType(GL_FLOAT);
    texcoordBuffer->SetUsage(GL_STATIC_DRAW);
    texcoordBuffer->SetNumberOfVertexAttribComponents(numberOfComponents);

    return texcoordBuffer;
}

VBO* createNormalVBO() {

    VBO* positionBuffer = new VBO();

    positionBuffer->SetVertexAttribIndex(VBO_NORMAL_ATTRIB_INDEX);
    positionBuffer->SetTarget(GL_ARRAY_BUFFER);
    positionBuffer->SetType(GL_FLOAT);
    positionBuffer->SetUsage(GL_STATIC_DRAW);
    positionBuffer->SetNumberOfVertexAttribComponents(3);

    return positionBuffer;
}

void VBO::EnableVertexAttrib() {
    LOG_I("set index: " +  tos(vertexAttribIndex));
    GL_C(glEnableVertexAttribArray(vertexAttribIndex));
    GL_C(glVertexAttribPointer(vertexAttribIndex, this->numberOfVertexAttribComponents, this->type, false, 0, 0));
}

void VBO::DisableVertexAttrib() {
    LOG_I("unset index: " +  tos(vertexAttribIndex));
    GL_C(glDisableVertexAttribArray(vertexAttribIndex));
}

void VBO::SetNumberOfVertexAttribComponents(GLint numberOfVertexAttribComponents) {

    LOG_I("num conponents : " +  tos(numberOfVertexAttribComponents));

    this->numberOfVertexAttribComponents = numberOfVertexAttribComponents;
}

void VBO::SetBufferData(GLsizeiptr size, const GLvoid* data) {

    LOG_I("size: " + tos(size));
    const float* fd = (const float*)data;
    LOG_I("fd[0]: " + std::to_string(fd[1]));


    GL_C(glBufferData(target,size, data, usage));
}
