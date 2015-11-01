#pragma once

#include "gl/gl_common.hpp"
#include "math/vector3f.hpp"

#include <string>

struct Material{
    std::string m_materialName;

    std::string m_textureFilename;
    std::string m_normalMapFilename;
    std::string m_specularMapFilename;
    std::string m_heightMapFilename;

    float m_shininess;
    Vector3f m_specularColor;

};

struct GeometryObjectData {

    struct Chunk{
	GLsizeiptr m_verticesSize;
	GLvoid* m_vertices;

	GLsizeiptr m_indicesSize;
	GLvoid* m_indices;

	GLuint m_numTriangles;

	Material* m_material; // store texture in material.
    };

    // every chunk is its own VBO, and has its own shader.
    std::vector<Chunk*> m_chunks;

    // how much data required for each vertex in the chunk.
    std::vector<GLuint> m_vertexAttribsSizes;

    // type of the integer used to store vertex indices.
    GLenum m_indexType;
};
