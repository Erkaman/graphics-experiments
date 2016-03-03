#include "ray_tracer.hpp"
#include "position_fbo.hpp"

#include "ewa/geometry_object_data.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/log.hpp"
#include "ewa/random.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/common.hpp"


#include <math.h>
#include <float.h>

using std::string;
using std::vector;

const int SHADOW_MAP_SIZE = 256*2*2;

RayTracer::RayTracer(GeometryObjectData* geoObj) {
    m_geoObj = geoObj;


    m_positionFbo = new PositionFbo();
    m_positionFbo->Init(0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);




/*
    LOG_I("attribs: %d", m_geoObj->m_vertexAttribsSizes.size() );
    LOG_I("attribs1: %d", m_geoObj->m_vertexAttribsSizes[0] );
    LOG_I("attribs2: %d", m_geoObj->m_vertexAttribsSizes[1] );
    LOG_I("attribs3: %d", m_geoObj->m_vertexAttribsSizes[2] );
*/
    m_vertexBuffer = VBO::CreateInterleaved(
	    m_geoObj->m_vertexAttribsSizes);

/*
    LOG_I("size: %d", m_geoObj->m_verticesSize );

    LOG_I("verticessize: %d", m_geoObj->m_verticesSize );
*/

    GLfloat* vs = (GLfloat *)m_geoObj->m_vertices;

    //  LOG_I("verticessize: %d", m_geoObj->m_verticesSize );

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(m_geoObj->m_verticesSize, vs /*m_geoObj->m_vertices*/);
    m_vertexBuffer->Unbind();

    for(size_t i = 0; i < m_geoObj->m_chunks.size(); ++i) {

	GeometryObjectData::Chunk* baseChunk = m_geoObj->m_chunks[i];

	Chunk newChunk;

	newChunk.m_indexBuffer = VBO::CreateIndex(m_geoObj->m_indexType);
	newChunk.m_numTriangles = baseChunk->m_numTriangles;

	unsigned short* indices = (unsigned short*)baseChunk->m_indices;

	newChunk.m_indexBuffer->Bind();
	newChunk.m_indexBuffer->SetBufferData(baseChunk->m_indicesSize, baseChunk->m_indices);
	newChunk.m_indexBuffer->Unbind();

	m_chunks.push_back(newChunk);
    }

    m_outputPosShader = ShaderProgram::Load("shader/pos_output");
    m_occlusionShader = ShaderProgram::Load("shader/occlusion");



}

GeometryObjectData* RayTracer::RayTrace() {



    static float xAngle = 30;
    static float yAngle = 80;


    KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {
	xAngle += 1.5f;
    }

    if( kbs.IsPressed(GLFW_KEY_L) ) {
	yAngle += 1.5f;
    }


/*
    LOG_I("aabbMin: %s",  string( m_geoObj->aabb.min ).c_str() );
    LOG_I("aabbMax: %s",  string( m_geoObj->aabb.max ).c_str() );
*/
    AABB aabb = m_geoObj->aabb;

	float bound =  0.5f;

    Matrix4f scale = Matrix4f::CreateScale(
	bound / (aabb.max.x - aabb.min.x),
	bound / (aabb.max.y - aabb.min.y),
	bound / (aabb.max.z - aabb.min.z)

	);

    Matrix4f translate = Matrix4f::CreateTranslation(
	-0.5f * (aabb.min.x + aabb.max.x),
	-0.5f * (aabb.min.y + aabb.max.y),
	-0.5f * (aabb.min.z + aabb.max.z)
	);

    Matrix4f modelMatrix = scale * translate;

    float size = 0.5f;

    Matrix4f projectionMatrix = Matrix4f::CreateOrthographic(
	    -size, +size,
	    -size, +size,
	    -size, +size
	    );

    int vertexCount =  m_geoObj->m_verticesSize / (sizeof(float) * (3+2+3));



    int vertexPosTextureSize = 2;

    // find smallest power of two.
    while(vertexPosTextureSize*vertexPosTextureSize < vertexCount) {
	vertexPosTextureSize *= 2;
    }


    m_occlusionFbos[0] = new PositionFbo();
    m_occlusionFbos[0]->Init(0, vertexPosTextureSize,vertexPosTextureSize);

    m_occlusionFbos[1] = new PositionFbo();
    m_occlusionFbos[1]->Init(0, vertexPosTextureSize,vertexPosTextureSize);




    /*
     LOG_I("vertexCount: %d",vertexCount );
     LOG_I("vertexPosTextureSize: %d",vertexPosTextureSize );
    */


     Vector3f* posBuffer = new Vector3f[vertexCount];


     GLfloat* vs = (GLfloat *)m_geoObj->m_vertices;

     for(int i = 0; i < vertexCount; ++i) {
	 posBuffer[i] = Vector3f(
	     vs[8 * i + 0],
	     vs[8 * i + 1],
	     vs[8 * i + 2]);

//	 LOG_I("vert: %s", string(posBuffer[i]).c_str()  );
     }

     Texture2D* m_vertexPosTexture = new Texture2D( (GLvoid *)posBuffer, vertexPosTextureSize,vertexPosTextureSize,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_vertexPosTexture->Bind();
    m_vertexPosTexture->SetTextureClamping();
    m_vertexPosTexture->SetMinFilter(GL_NEAREST);
    m_vertexPosTexture->SetMagFilter(GL_NEAREST);
    m_vertexPosTexture->Unbind();





    int occlusionIndex = 0;




	Matrix4f viewMatrix =
	    Matrix4f::CreateRotate(xAngle, Vector3f(1,0,0) ) *
	    Matrix4f::CreateRotate(yAngle, Vector3f(0,1,0) );

/*
    m_positionFbo->Bind();
    {
	m_outputPosShader->Bind();

	GL_C(glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE));
	GL_C(glClearColor(1.0f, 0.0f, 0.0f, 1.0f));
	GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));




	m_outputPosShader->SetUniform("modelMatrix", modelMatrix);
	m_outputPosShader->SetUniform("viewMatrix", viewMatrix);
	m_outputPosShader->SetUniform("projectionMatrix", projectionMatrix);

	m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

	for(const Chunk& chunk : m_chunks) {
	    chunk.m_indexBuffer->Bind();
	    chunk.m_indexBuffer->DrawIndices(GL_TRIANGLES, (chunk.m_numTriangles)*3);
	    chunk.m_indexBuffer->Unbind();
	}

	m_vertexBuffer->DisableVertexAttribInterleavedWithBind();


	m_outputPosShader->Unbind();

    }
    m_positionFbo->Unbind();
    */


    occlusionIndex = 1 - occlusionIndex;
    PositionFbo* fboSource = m_occlusionFbos[1 - occlusionIndex];
    PositionFbo* fboDest = m_occlusionFbos[occlusionIndex];


    m_occlusionShader->Bind();


    m_occlusionShader->SetUniform("vertexPosMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_vertexPosTexture->Bind();


    fboDest->Bind();

    GL_C(glViewport(0, 0, vertexPosTextureSize, vertexPosTextureSize));
    GL_C(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


/*
    m_occlusionShader->SetUniform("modelMatrix", modelMatrix);
    m_occlusionShader->SetUniform("viewMatrix", viewMatrix);
    m_occlusionShader->SetUniform("projectionMatrix", projectionMatrix);
*/

    // fullscreen.
    // GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));



    fboDest->Unbind();


    m_vertexPosTexture->Unbind();


    m_occlusionShader->Unbind();


    //  GL_C(glFlush() );
    // GL_C(glFinish() );


//    fboDest->Bind();

/*
    if(fboDest->GetRenderTargetTexture() == NULL) {
	LOG_I("YLL");
    }
*/



    float* pixels = fboDest->GetRenderTargetTexture().GetPixels<float>(
	vertexPosTextureSize * vertexPosTextureSize *  2, GL_RGBA, GL_FLOAT  );


//    fboDest->Unbind();
    LOG_I("RETURN");

    MY_DELETE(pixels);

    LOG_I("RETURN");

    MY_DELETE(fboDest);

    return m_geoObj;
}
