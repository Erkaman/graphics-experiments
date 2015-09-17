#pragma once

#include "gl/gl_common.hpp"

#include <string>

struct Material{
    std::string m_textureFilename;
};

struct GeometryObjectData {

    // make the types of these as generic as possible.

    struct Chunk{
	GLsizeiptr m_verticesSize;
	GLvoid* m_vertices;

	GLsizeiptr m_indicesSize;
	GLvoid* m_indices;

	GLuint m_numTriangles;


	Material m_material; // store texture in material.
    };

    std::vector<Chunk*> m_chunks;

    std::vector<GLuint> m_vertexAttribsSizes;

    // type of the integer used to store vertex indices.
    GLenum m_indexType;

    int m_hasNormalMaps;
};
