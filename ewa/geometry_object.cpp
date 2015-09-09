#include "geometry_object.hpp"

#include "math/vector3f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/common.hpp"
#include "ewa/file.hpp"
#include "eob_file.hpp"

using std::string;

void GeometryObject::Init(const std::string& filename, const bool useCustomShader) {
    GeometryObjectData data = EobFile::Read(filename);

    string basePath = File::GetFilePath(filename);

    for(size_t i = 0; i < data.m_chunks.size(); ++i) {
	Material& mat = data.m_chunks[i]->m_material;

	mat.m_textureFilename = File::AppendPaths(basePath, mat.m_textureFilename);
    }


    Init(data, useCustomShader);
}


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


	if(baseChunk->m_material.m_textureFilename != "") {


	    Texture* texture = new Texture2D(baseChunk->m_material.m_textureFilename);

	    texture->Bind();
	    texture->SetTextureClamping();
	    texture->GenerateMipmap();
	    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	    texture->SetMagFilter(GL_LINEAR);
	    texture->Unbind();

	    newChunk->m_texture = texture;
	} else {
	    newChunk->m_texture = NULL;
	}

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

    RenderVertices(*m_defaultShader);

    m_defaultShader->Unbind();

}

void GeometryObject::RenderVertices(ShaderProgram& shader) {

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];


	if(chunk->m_texture != NULL) {
	    shader.SetUniform("tex", 0);
	    Texture::SetActiveTextureUnit(0);
	    chunk->m_texture->Bind();
	}

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);

	if(chunk->m_texture != NULL) {
	    chunk->m_texture->Unbind();
	}

    }

}
