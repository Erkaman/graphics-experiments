#pragma once

#include "gl_common.hpp"

class VAO {

private:
    GLuint vao;

    VAO(const VAO&);
    VAO& operator=(const VAO&);


public:

    VAO() {
        GL_C(glGenVertexArrays(1, &vao));
    }

    ~VAO() {
        GL_C(glDeleteVertexArrays(1, &vao));
    }


    void Bind() {
        GL_C(glBindVertexArray(vao));
    }

    void Unbind() {
        GL_C(glBindVertexArray(0));
    }

};
