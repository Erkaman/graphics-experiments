#pragma once

#include "gl_common.hpp"


class Query {

private:

    GLenum m_target;
    GLuint m_id;


public:

    Query(GLenum target);
    ~Query();

    void Bind();
    void Unbind();

    GLuint GetResult();


};
