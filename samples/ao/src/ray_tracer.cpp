#include "ray_tracer.hpp"
#include "position_fbo.hpp"

#include "ewa/geometry_object_data.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/log.hpp"
#include "ewa/random.hpp"
#include "ewa/math/matrix4f.hpp"

#include <math.h>
#include <float.h>

using std::string;
using std::vector;

const int SHADOW_MAP_SIZE = 256*2*2;

RayTracer::RayTracer(GeometryObjectData* geoObj) {
    m_geoObj = geoObj;


    m_positionFbo = new PositionFbo();
    m_positionFbo->Init(0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);


    m_vertexBuffer = VBO::CreateInterleaved(
	    m_geoObj->m_vertexAttribsSizes);


    LOG_I("size: %d", m_geoObj->m_verticesSize );

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(m_geoObj->m_verticesSize, m_geoObj->m_vertices);
    m_vertexBuffer->Unbind();

    for(size_t i = 0; i < m_geoObj->m_chunks.size(); ++i) {

	GeometryObjectData::Chunk* baseChunk = m_geoObj->m_chunks[i];

	Chunk newChunk;

	newChunk.m_indexBuffer = VBO::CreateIndex(m_geoObj->m_indexType);
	newChunk.m_numTriangles = baseChunk->m_numTriangles;

	newChunk.m_indexBuffer->Bind();
	newChunk.m_indexBuffer->SetBufferData(baseChunk->m_indicesSize, baseChunk->m_indices);
	newChunk.m_indexBuffer->Unbind();

	m_chunks.push_back(newChunk);
    }

    m_shader = ShaderProgram::Load("shader/pos_output");
}

GeometryObjectData* RayTracer::RayTrace() {


/*
    LOG_I("aabbMin: %s",  string( m_geoObj->aabb.min ).c_str() );
    LOG_I("aabbMax: %s",  string( m_geoObj->aabb.max ).c_str() );
*/
    AABB aabb = m_geoObj->aabb;

//    m_positionFbo->Bind();
    {
	m_shader->Bind();

	GL_C(glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE));
	GL_C(glClearColor(1.0f, 0.0f, 0.0f, 1.0f));
	GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	Matrix4f translate = Matrix4f::CreateTranslation(
	    -0.5f * (aabb.min.x + aabb.max.x),
	    -0.5f * (aabb.min.y + aabb.max.y),
	    -0.5f * (aabb.min.z + aabb.max.z)
	    );


	float bound = 0.9f;

	Matrix4f scale = Matrix4f::CreateScale(
	    bound / (aabb.max.x - aabb.min.x),
	    bound / (aabb.max.y - aabb.min.y),
	    bound / (aabb.max.z - aabb.min.z)

	    );


/*
	LOG_I("boundx: %f", 	    bound / (aabb.max.x - aabb.min.x));
	LOG_I("boundy: %f", 	    bound / (aabb.max.y - aabb.min.y));
	LOG_I("boundz: %f", 	    bound / (aabb.max.z - aabb.min.z));
*/

	Matrix4f modelMatrix = Matrix4f::CreateIdentity();//scale * translate;

	float xAngle = 60;
	float yAngle = 10;
/*
	Matrix4f viewMatrix =
	    Matrix4f::CreateRotate(xAngle, Vector3f(1,0,0) ) *
	    Matrix4f::CreateRotate(yAngle, Vector3f(0,1,0) );
*/
	Matrix4f viewMatrix = Matrix4f::CreateIdentity();

	float size = 10.5f;

	Matrix4f projectionMatrix = Matrix4f::CreateOrthographic(
	    -size, +size,
	    -size, +size,
	    -size, +size

	    );

/*
	Matrix4f modelMatrix = geoObj->GetModelMatrix( );

	    const Matrix4f mvp = lightVp * modelMatrix;
	    outputDepthShader->SetUniform("mvp", mvp  );
*/
	m_shader->SetUniform("modelMatrix", modelMatrix);
	m_shader->SetUniform("viewMatrix", viewMatrix);
	m_shader->SetUniform("projectionMatrix", projectionMatrix);

	::SetCullFace(false);

	for(const Chunk& chunk : m_chunks) {
//	    LOG_I("lol: %d",  chunk.m_numTriangles);


	    chunk.m_indexBuffer->Bind();
	    chunk.m_indexBuffer->DrawIndices(GL_TRIANGLES, (chunk.m_numTriangles)*3);
	    chunk.m_indexBuffer->Unbind();
	}

	m_shader->Unbind();

    }
    //  m_positionFbo->Unbind();


    return m_geoObj;
}
