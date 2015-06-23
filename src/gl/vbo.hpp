#pragma once

#include "gl_common.hpp"


class VBO {

private:

    GLuint buffer;
    GLuint vertexAttribIndex;
    GLenum target;
    GLenum type;
    GLint numberOfVertexAttribComponents;
    GLenum usage;

public:
    VBO() {
	GL_C(glGenBuffers(1, &buffer));
    }

    void Dispose() {
	GL_C(glDeleteBuffers(1, &buffer));
    }

    void SetVertexAttribIndex(GLuint vertexAttribIndex) {
        this->vertexAttribIndex = vertexAttribIndex;
    }

    void SetNumberOfVertexAttribComponents(GLint numberOfVertexAttribComponents);

    void SetTarget(GLenum target) {
        this->target = target;
    }

    void SetBufferData(GLsizeiptr size, const GLvoid* data);

    void EnableVertexAttrib();

    void DisableVertexAttrib();

    void Bind() {
        GL_C(glBindBuffer(this->target, this->buffer));
    }

    void Unbind() {
        GL_C(glBindBuffer(target, 0));
    }

    void SetType(GLenum type) {
        this->type = type;
    }

    void SetUsage(GLenum usage) {
        this->usage = usage;
    }
};

VBO* createPositionVBO(GLint numComponents);
VBO* createIndexVBO();
VBO* createTexCoordVBO(GLint numberOfComponents);
VBO* createNormalVBO();
