#pragma once

#include "gl/gl_common.hpp"

struct GeometryObjectData {

    FloatVector m_vertices;
    UshortVector m_indices;

    GLuint m_numTriangles;

    std::vector<GLuint> m_vertexAttribsSizes;

    // type of the integer used to store vertex indices.
    GLenum m_indexType;

};
