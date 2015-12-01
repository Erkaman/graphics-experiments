#include "geometry_object.hpp"

#include "math/vector3f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/common.hpp"
#include "ewa/camera.hpp"
#include "ewa/file.hpp"
#include "eob_file.hpp"
#include "resource_manager.hpp"

using std::string;
using std::vector;

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

    GeometryObjectData data = EobFile::Read(filename);


    string basePath = File::GetFilePath(filename);

    m_hasNormalMap = false;
    m_hasSpecularMap = false;
    m_hasHeightMap = false;

    for(size_t i = 0; i < data.m_chunks.size(); ++i) {
	Material* mat = data.m_chunks[i]->m_material;

	if(mat->m_textureFilename != ""){ // empty textures should remain empty.
	    mat->m_textureFilename = File::AppendPaths(basePath, mat->m_textureFilename);
	}

	if(mat->m_normalMapFilename != ""){ // empty textures should remain empty->
	    mat->m_normalMapFilename = File::AppendPaths(basePath, mat->m_normalMapFilename);
	    m_hasNormalMap = true;
	    m_hasHeightMap = mat->m_hasHeightMap;
	}

	if(mat->m_specularMapFilename != ""){ // empty textures should remain empty->
	    mat->m_specularMapFilename = File::AppendPaths(basePath, mat->m_specularMapFilename);
	    m_hasSpecularMap = true;
	}

    }

    Init(data, useCustomShader);
}


void GeometryObject::Init(GeometryObjectData& data, const bool useCustomShader) {

    if(!useCustomShader) {
	string shaderName = "shader/simple";

	vector<string> defines;

	if(m_hasSpecularMap) {
	    defines.push_back("SPEC_MAPPING");
	}

	if(m_hasHeightMap) {
	    defines.push_back("HEIGHT_MAPPING");
	} else if(m_hasNormalMap) { // only a normal map, no height map.
	    defines.push_back("NORMAL_MAPPING");
	}

	m_defaultShader = ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }

    m_depthShader = new ShaderProgram("shader/output_depth");


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


	if(baseChunk->m_material->m_textureFilename != "") {
	    newChunk->m_texture = LoadTexture(baseChunk->m_material->m_textureFilename);
	} else {
	    newChunk->m_texture = NULL;
	}

	if(m_hasNormalMap) {
	    newChunk->m_normalMap = LoadTexture(baseChunk->m_material->m_normalMapFilename);
	} else {
	    newChunk->m_normalMap = NULL;
	}

	if(m_hasSpecularMap) {
	    newChunk->m_specularMap = LoadTexture(baseChunk->m_material->m_specularMapFilename);
	} else {
	    newChunk->m_specularMap = NULL;
	}

	newChunk->m_shininess = baseChunk->m_material->m_shininess;
	newChunk->m_specularColor = baseChunk->m_material->m_specularColor;

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

void GeometryObject::RenderShadowMap(const Matrix4f& lightVp) {

    m_depthShader->Bind();

    const Matrix4f mvp = lightVp * m_modelMatrix;
    m_depthShader->SetUniform("mvp", mvp  );

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

    m_depthShader->Unbind();

}

void GeometryObject::Render(const Camera& camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {

    m_defaultShader->Bind();

    m_defaultShader->SetPhongUniforms(

	m_modelMatrix
	, camera, lightPosition,
	lightVp);

    m_defaultShader->SetUniform("shadowMap", (int)shadowMap.GetTargetTextureUnit() );
    Texture::SetActiveTextureUnit(shadowMap.GetTargetTextureUnit());
    shadowMap.GetRenderTargetTexture().Bind();


    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];


	if(chunk->m_texture != NULL) {
	    m_defaultShader->SetUniform("diffMap", 0);
	    Texture::SetActiveTextureUnit(0);
	    chunk->m_texture->Bind();
	}

	if(chunk->m_normalMap != NULL) {
	    m_defaultShader->SetUniform("normalMap", 1);
	    Texture::SetActiveTextureUnit(1);
	    chunk->m_normalMap->Bind();
	}

	if(chunk->m_specularMap != NULL) {
	    m_defaultShader->SetUniform("specMap", 2);
	    Texture::SetActiveTextureUnit(2);
	    chunk->m_specularMap->Bind();
	} else {
	    // if no spec map, the model has the same specular color all over the texture.
	    m_defaultShader->SetUniform("specColor", chunk->m_specularColor);
	}

	m_defaultShader->SetUniform("specShiny", chunk->m_shininess);

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);

	if(chunk->m_texture != NULL) {
	    chunk->m_texture->Unbind();
	}

	if(chunk->m_normalMap != NULL) {
	    chunk->m_normalMap->Unbind();
	}

	if(chunk->m_specularMap != NULL) {
	    chunk->m_specularMap->Unbind();
	}
    }

    shadowMap.GetRenderTargetTexture().Unbind();


    m_defaultShader->Unbind();

}

void  GeometryObject::SetModelMatrix(Matrix4f modelMatrix) {
    m_modelMatrix = modelMatrix;
}
