#pragma once

#include "precomp.h"

class VBO {

private:

    GLuint buffer;
    GLuint vertexAttribIndex;
    GLenum target;
    GLenum type;
    GLint numberOfVertexAttribComponents;
    GLenum usage;

    VBO() {
	glGenBuffers(1, &buffer);
    }

    void Dispose() {
	glDeleteBuffers(1, &buffer);
    }

    void SetVertexAttribIndex(GLuint vertexAttribIndex) {
        this->vertexAttribIndex = vertexAttribIndex;
    }

    void SetNumberOfVertexAttribComponents(GLint numberOfVertexAttribComponents) {
        this->numberOfVertexAttribComponents = numberOfVertexAttribComponents;
    }

    void SetTarget(GLenum target) {
        this->target = target;
    }

    void SetBufferData(GLsizeiptr size, const GLvoid* data) {
        glBufferData(target,size, data, usage);
    }

    void EnableVertexAttrib() {
        glEnableVertexAttribArray(vertexAttribIndex);
        glVertexAttribPointer(vertexAttribIndex, this->numberOfVertexAttribComponents, this->type, false, 0, 0);
    }

    void disableVertexAttrib() {
        glDisableVertexAttribArray(vertexAttribIndex);
    }

    void Bind() {
        glBindBuffer(this->target, this->buffer);
    }

    void Unbind() {
        glBindBuffer(target, 0);
    }


    void SetType(GLenum type) {
        this->type = type;
    }

    void SetUsage(GLenum usage) {
        this->usage = usage;
    }
};
