#include "geometry_object.hpp"

#include "math/vector3f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/common.hpp"
#include "ewa/file.hpp"
#include "eob_file.hpp"
#include "resource_manager.hpp"

using std::string;

Texture* LoadTexture(const string& filename) {
    Texture* texture = new Texture2D(filename);

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    return texture;
}

void GeometryObject::Init(const std::string& filename, const bool useCustomShader) {

    m_bumpOn = 1;

    GeometryObjectData data = EobFile::Read(filename);


    string basePath = File::GetFilePath(filename);


    for(size_t i = 0; i < data.m_chunks.size(); ++i) {
	Material& mat = data.m_chunks[i]->m_material;

	if(mat.m_textureFilename != "") // empty textures should remain empty.
	    mat.m_textureFilename = File::AppendPaths(basePath, mat.m_textureFilename);

	if(mat.m_normalMapFilename != "") // empty textures should remain empty.
	    mat.m_normalMapFilename = File::AppendPaths(basePath, mat.m_normalMapFilename);

	if(mat.m_specularMapFilename != "") // empty textures should remain empty.
	    mat.m_specularMapFilename = File::AppendPaths(basePath, mat.m_specularMapFilename);
    }


    Init(data, useCustomShader);
}


void GeometryObject::Init(GeometryObjectData& data, const bool useCustomShader) {


    if(!useCustomShader) {
	string shaderName = "shader/simple";
	string vertexSource = ResourceManager::LocateAndReadResource(shaderName + "_vs.glsl");
	string fragmentSource = ResourceManager::LocateAndReadResource(shaderName + "_fs.glsl");

	if(data.m_hasNormalMaps) {

	    vertexSource = string("#define NORMAL_MAPPING\n\n") + vertexSource;
	    fragmentSource = string("#define NORMAL_MAPPING\n\n") + fragmentSource;

	    vertexSource = string("#define SPEC_MAPPING\n\n") + vertexSource;
	    fragmentSource = string("#define SPEC_MAPPING\n\n") + fragmentSource;

	}

	m_defaultShader = new ShaderProgram(vertexSource, fragmentSource);
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
	    newChunk->m_texture = LoadTexture(baseChunk->m_material.m_textureFilename);
	} else {
	    newChunk->m_texture = NULL;
	}

	if(data.m_hasNormalMaps) {
	    newChunk->m_normalMap = LoadTexture(baseChunk->m_material.m_normalMapFilename);
	} else {
	    newChunk->m_normalMap = NULL;
	}

	if(data.m_hasSpecularMaps) {
	    newChunk->m_specularMap = LoadTexture(baseChunk->m_material.m_specularMapFilename);
	} else {
	    newChunk->m_specularMap = NULL;
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

	m_defaultShader->SetUniform("bump_on", m_bumpOn);

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

	if(chunk->m_normalMap != NULL) {
	    shader.SetUniform("normalMap", 1);
	    Texture::SetActiveTextureUnit(1);
	    chunk->m_normalMap->Bind();
	}

	if(chunk->m_specularMap != NULL) {
	    shader.SetUniform("specularMap", 2);
	    Texture::SetActiveTextureUnit(2);
	    chunk->m_specularMap->Bind();
	}


	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);

	if(chunk->m_texture != NULL) {
	    chunk->m_texture->Unbind();
	}

    }

}


void  GeometryObject::SetModelMatrix(Matrix4f modelMatrix) {
    m_modelMatrix = modelMatrix;
}


void GeometryObject::ToggleBump(int bump) {
    m_bumpOn = bump;
}
