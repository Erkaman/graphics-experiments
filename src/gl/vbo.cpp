#include "vbo.h"

VBO createPositionVBO(GLint numComponents) {
    VBO positionBuffer;

    // configure VBO.
    positionBuffer.SetVertexAttribIndex(0);
    positionBuffer.SetTarget(GL_ARRAY_BUFFER);
    positionBuffer.SetType(GL_FLOAT);
    positionBuffer.SetUsage(GL_STATIC_DRAW);
    positionBuffer.SetNumberOfVertexAttribComponents(numComponents);

    return positionBuffer;
}

VBO createIndexVBO() {
    VBO indexBuffer;

    /**
     * Configure buffer.
     */
    indexBuffer.SetTarget(GL_ELEMENT_ARRAY_BUFFER);
    indexBuffer.SetType(GL_INT);
    indexBuffer.SetUsage(GL_STATIC_DRAW);

    return indexBuffer;
}

VBO createTexCoordVBO(GLint numberOfComponents) {
    VBO texcoordBuffer;

    // configure buffer
    texcoordBuffer.SetVertexAttribIndex(2);
    texcoordBuffer.SetTarget(GL_ARRAY_BUFFER);
    texcoordBuffer.SetType(GL_FLOAT);
    texcoordBuffer.SetUsage(GL_STATIC_DRAW);
    texcoordBuffer.SetNumberOfVertexAttribComponents(numberOfComponents);

    return texcoordBuffer;
}


VBO createNormalVBO() {

    VBO positionBuffer;

    positionBuffer.SetVertexAttribIndex(1);
    positionBuffer.SetTarget(GL_ARRAY_BUFFER);
    positionBuffer.SetType(GL_FLOAT);
    positionBuffer.SetUsage(GL_STATIC_DRAW);
    positionBuffer.SetNumberOfVertexAttribComponents(3);

    return positionBuffer;
}
