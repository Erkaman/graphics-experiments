#include "geometry_object.hpp"

#include "math/vector3f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/common.hpp"

void GeometryObject::Init(GeometryObjectData& data, const bool useCustomShader) {

    if(!useCustomShader) {
	m_defaultShader = new ShaderProgram("shader/simple");
    }

    for(size_t i = 0; i < data.m_chunks.size(); ++i) {
	GeometryObjectData::Chunk* baseChunk = data.m_chunks[i];

	Chunk* newChunk = new Chunk;

	newChunk->m_vertexBuffer = VBO::CreateInterleaved(
	    data.m_vertexAttribsSizes);

	newChunk->m_indexBuffer = VBO::CreateIndex(data.m_indexType);


	newChunk->m_numTriangles = baseChunk->m_numTriangles;

	newChunk->m_vertexBuffer->Bind();
	newChunk->m_vertexBuffer->SetBufferData(baseChunk->m_verticesSize, baseChunk->m_vertices);
	newChunk->m_vertexBuffer->Unbind();

	newChunk->m_indexBuffer->Bind();
	newChunk->m_indexBuffer->SetBufferData(baseChunk->m_indicesSize, baseChunk->m_indices);
	newChunk->m_indexBuffer->Unbind();

	m_chunks.push_back(newChunk);
    }
}


GeometryObject::~GeometryObject() {

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	MY_DELETE(chunk->m_vertexBuffer);
	MY_DELETE(chunk->m_indexBuffer);

	MY_DELETE(chunk);
    }
}


void GeometryObject::Render(const Camera& camera, const Vector4f& lightPosition) {
        m_defaultShader->Bind();

    m_defaultShader->SetPhongUniforms(

	m_modelMatrix
	 , camera, lightPosition);

    RenderVertices();

    m_defaultShader->Unbind();

}

void GeometryObject::RenderVertices() {

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];


	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

}
