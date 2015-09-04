#pragma once

#include "gl/gl_common.hpp"

struct GeometryObjectData {

    // make the types of these as generic as possible.
    GLsizeiptr m_verticesSize;
    GLvoid* m_vertices;

    GLsizeiptr m_indicesSize;
    GLvoid* m_indices;

    GLuint m_numTriangles;

    std::vector<GLuint> m_vertexAttribsSizes;

    // type of the integer used to store vertex indices.
    GLenum m_indexType;

};
