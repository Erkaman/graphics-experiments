#pragma once

#include "precomp.h"

class VAO {

private:
    GLuint vao;

public:

    VAO() {
        GL_C(glGenVertexArrays(1, &vao));
    }

    void Bind() {
        GL_C(glBindVertexArray(vao));
    }

    void Unbind() {
        GL_C(glBindVertexArray(0));
    }

    void Dispose () {
        GL_C(glDeleteVertexArrays(1, &vao));
    }
};
