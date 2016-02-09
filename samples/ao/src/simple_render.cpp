#include "simple_render.hpp"

#include "ewa/resource_manager.hpp"
#include "ewa/geometry_object_data.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/gl/array_texture.hpp"

#include "ewa/math/matrix4f.hpp"

using std::string;
using std::vector;


struct Chunk {
    VBO* m_indexBuffer;
    GLuint m_numTriangles;

    float m_shininess;
    Vector3f m_specularColor;
    Vector3f m_diffuseColor;

    // the material.
    GLint m_texture;
    GLint m_specularMap;
};

SimpleRender::SimpleRender() {
    m_eob = nullptr;

	{
	    string shaderName = "shader/geo_obj_render";

	    vector<string> defines;

	    defines.push_back("DIFF_MAPPING");
	    defines.push_back("ALPHA_MAPPING");
	    defines.push_back("DIFFUSE_LIGHT");

	    m_simpleShader = ResourceManager::LoadShader(
		shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
	}


	{
	    string shaderName = "shader/geo_obj_render";

	    vector<string> defines;

	    defines.push_back("DIFF_MAPPING");
	    defines.push_back("ALPHA_MAPPING");
	    defines.push_back("DIFFUSE_LIGHT");
	    defines.push_back("AO");

	    m_aoShader = ResourceManager::LoadShader(
		shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
	}

	vector<string> files = File::EnumerateDirectory("../game/obj");

        vector<string> pngFiles;

	for(const string& file: files) {

	    if(file.size() > 4) { // must be long enough to fit a ".png extension.

		if(file.substr(file.size()-3).c_str() == string("png") ) {

		    string fullpathFile = File::AppendPaths("obj", file);

		    LOG_I("res: %s", fullpathFile.c_str());

		    pngFiles.push_back(fullpathFile);


		}
	    }
	}

	m_arrayTexture = ArrayTexture::Load(pngFiles);

	m_arrayTexture->Bind();
	m_arrayTexture->SetTextureRepeat();
	m_arrayTexture->GenerateMipmap();
	m_arrayTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_arrayTexture->SetMagFilter(GL_LINEAR);
	m_arrayTexture->Unbind();


	if(!m_arrayTexture) {
	    PrintErrorExit();
	}

}

void SimpleRender::SetEob(GeometryObjectData* eob, const string& basePath) {
    m_eob = eob;




    m_vertexBuffer = VBO::CreateInterleaved(
	eob->m_vertexAttribsSizes);
    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(eob->m_verticesSize, eob->m_vertices);
    m_vertexBuffer->Unbind();

    if(eob->m_vertexAttribsSizes[0] == 4 ) {
	isAo = true;
    } else {
	isAo = false;
    }

    /*
      Next, we create VBOs from the vertex data in the chunks.
    */
    for(size_t i = 0; i < eob->m_chunks.size(); ++i) {

	GeometryObjectData::Chunk* baseChunk = eob->m_chunks[i];
	Chunk* newChunk = new Chunk;




	newChunk->m_texture = -1;
	newChunk->m_specularMap = -1;


	Material* mat = eob->m_chunks[i]->m_material;

	if(mat->m_textureFilename != ""){ // empty textures should remain empty.

	    newChunk->m_texture =
		m_arrayTexture->GetTexture(File::AppendPaths(basePath, mat->m_textureFilename));

	} else {
	    newChunk->m_texture = -1;
	}

	if(mat->m_specularMapFilename != ""){ // empty textures should remain empty->



	    newChunk->m_specularMap =
		m_arrayTexture->GetTexture(File::AppendPaths(basePath, mat->m_specularMapFilename));


	}



	newChunk->m_indexBuffer = VBO::CreateIndex(eob->m_indexType);


	newChunk->m_numTriangles = baseChunk->m_numTriangles;



	newChunk->m_indexBuffer->Bind();
	newChunk->m_indexBuffer->SetBufferData(baseChunk->m_indicesSize, baseChunk->m_indices);
	newChunk->m_indexBuffer->Unbind();

	newChunk->m_shininess = baseChunk->m_material->m_specularExponent;
	newChunk->m_specularColor = baseChunk->m_material->m_specularColor;
	newChunk->m_diffuseColor = baseChunk->m_material->m_diffuseColor;

	string shaderName = "shader/geo_obj_render";

	m_chunks.push_back(newChunk);
    }


}


void SimpleRender::Render(ICamera* camera, const Vector4f& lightPosition) {

    ShaderProgram* shader = isAo ? m_aoShader : m_simpleShader;

    // bind shader of the batch.
    shader->Bind();

    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    shader->SetUniform("textureArray", 0);
    Texture::SetActiveTextureUnit(0);
    m_arrayTexture->Bind();


    shader->SetUniform("aoOnly", 1.0f);


    Matrix4f modelMatrix = Matrix4f::CreateIdentity(); //geoObj->GetModelMatrix();

    shader->SetPhongUniforms(
	modelMatrix
	, camera, lightPosition);

    for(size_t i = 0; i < m_chunks.size(); ++i) {

	Chunk* chunk = m_chunks[i];

	if(chunk->m_specularMap == -1) {
	    // if no spec map, the chunk has the same specular color all over the texture.
	    shader->SetUniform("specColor", chunk->m_specularColor);
	}

	shader->SetUniform("specShiny", chunk->m_shininess);

	if(chunk->m_texture != -1) {
	    shader->SetUniform("diffMap", (float)chunk->m_texture  );
	}

	chunk->m_indexBuffer->Bind();
	chunk->m_indexBuffer->DrawIndices(GL_TRIANGLES, (chunk->m_numTriangles)*3);
	chunk->m_indexBuffer->Unbind();
    }


    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();

    m_arrayTexture->Unbind();

}
