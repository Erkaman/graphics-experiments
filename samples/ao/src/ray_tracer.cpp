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





    LOG_I("attribs: %d", m_geoObj->m_vertexAttribsSizes.size() );
    LOG_I("attribs1: %d", m_geoObj->m_vertexAttribsSizes[0] );
    LOG_I("attribs2: %d", m_geoObj->m_vertexAttribsSizes[1] );
    LOG_I("attribs3: %d", m_geoObj->m_vertexAttribsSizes[2] );

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



bool first = true;

GeometryObjectData* RayTracer::RayTrace() {





    KeyboardState& kbs = KeyboardState::GetInstance();
/*
    if( kbs.IsPressed(GLFW_KEY_1) ) {
	xAngle += 1.5f;
    }

    if( kbs.IsPressed(GLFW_KEY_2) ) {
	yAngle += 1.5f;
    }
*/
//    xAngle += 10.0f;
//    LOG_I("angle: %f", xAngle );

    AABB aabb = m_geoObj->aabb;

	float bound =  0.5f;

    Matrix4f scale = Matrix4f::CreateScale(
	bound / (aabb.m_max.x - aabb.m_min.x),
	bound / (aabb.m_max.y - aabb.m_min.y),
	bound / (aabb.m_max.z - aabb.m_min.z)

	);

    Matrix4f translate = Matrix4f::CreateTranslation(
	-0.5f * (aabb.m_min.x + aabb.m_max.x),
	-0.5f * (aabb.m_min.y + aabb.m_max.y),
	-0.5f * (aabb.m_min.z + aabb.m_max.z)
	);

    Matrix4f modelMatrix = scale * translate;

    float size = 0.5f;

    Matrix4f projectionMatrix = Matrix4f::CreateOrthographic(
	    -size, +size,
	    -size, +size,
	    -size, +size
	    );



    int vertexCount =  m_geoObj->m_verticesSize / (sizeof(float) * (3+2+3+  (first ? 0 : 1)  ));


    int vertexPosTextureSize = 2;

    // find smallest power of two.
    while(vertexPosTextureSize*vertexPosTextureSize < vertexCount) {
	vertexPosTextureSize *= 2;
    }


    m_occlusionFbos[0] = new PositionFbo();
    m_occlusionFbos[0]->Init(0, vertexPosTextureSize,vertexPosTextureSize);

    m_occlusionFbos[1] = new PositionFbo();
    m_occlusionFbos[1]->Init(0, vertexPosTextureSize,vertexPosTextureSize);


     Vector3f* posBuffer = new Vector3f[vertexCount];


     GLfloat* vs = (GLfloat *)m_geoObj->m_vertices;


     for(int i = 0; i < vertexCount; ++i) {

	 int size = first ? 8 : 9;

	 posBuffer[i] = Vector3f(
	     vs[size * i + 0],
	     vs[size * i + 1],
	     vs[size * i + 2]);
/*
	 if(i < 20)
	     LOG_I("vert: %s", string(posBuffer[i]).c_str()  );
	 */
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

    Random r(1100);

    const int NUM_SAMPLES = 1;

    PositionFbo* fboSource;
    PositionFbo* fboDest;

    for(int i = 0; i < NUM_SAMPLES; ++i) {

	float xAngle = r.RandomFloat(0, 360);
	float yAngle = r.RandomFloat(0, 360);


	Matrix4f viewMatrix =
	    Matrix4f::CreateRotate(xAngle, Vector3f(1,0,0) ) *
	    Matrix4f::CreateRotate(yAngle, Vector3f(0,1,0) );

	occlusionIndex = 1 - occlusionIndex;
	fboSource = m_occlusionFbos[1 - occlusionIndex];
	fboDest = m_occlusionFbos[occlusionIndex];



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




//	begin occ






	m_occlusionShader->Bind();


	m_occlusionShader->SetUniform("vertexPosMap", 0);
	Texture::SetActiveTextureUnit(0);
	m_vertexPosTexture->Bind();


	m_occlusionShader->SetUniform("positionFbo", 1);
	Texture::SetActiveTextureUnit(1);
	m_positionFbo->GetRenderTargetTexture().Bind();

	m_occlusionShader->SetUniform("fboSource", 2);
	Texture::SetActiveTextureUnit(2);
	fboSource->GetRenderTargetTexture().Bind();

	m_occlusionShader->SetUniform("numSamples", (float)NUM_SAMPLES);



	fboDest->Bind();

	GL_C(glViewport(0, 0, vertexPosTextureSize, vertexPosTextureSize));
	GL_C(glClearColor(0.0f, 1.0f, 0.0f, 1.0f));
	GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));



	m_occlusionShader->SetUniform("modelMatrix", modelMatrix);
	m_occlusionShader->SetUniform("viewMatrix", viewMatrix);
	m_occlusionShader->SetUniform("projectionMatrix", projectionMatrix);

	// fullscreen.
	GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));


	fboDest->Unbind();


	m_vertexPosTexture->Unbind();

	m_occlusionShader->Unbind();

    }

    float* pixels = fboDest->GetRenderTargetTexture().GetPixels<float>(
	vertexPosTextureSize * vertexPosTextureSize *  4, GL_RGBA, GL_FLOAT  );

    LOG_I("pix: %f", pixels[0] );
    LOG_I("pix: %f", pixels[1] );
     LOG_I("pix: %f", pixels[2] );
    LOG_I("pix: %f", pixels[3] );
    LOG_I("pix: %f", pixels[4] );
    LOG_I("pix: %f", pixels[5] );
     LOG_I("pix: %f", pixels[6] );
    LOG_I("pix: %f", pixels[7] );


     struct Vertex {
	Vector3f point;
	Vector2f texCoord;
	Vector3f normal;
    };



   struct AoVertex {
	Vector4f point;
	Vector2f texCoord;
	Vector3f normal;
    };


    // LOG_I("num vertices: %d", vertices.size());

       Vertex* verticesBuffer = (Vertex *)m_geoObj->m_vertices;

       const int vertexSize = (3+2+3 + (first ? 0 : 1) ) * sizeof(float);
    const int numVertices = m_geoObj->m_verticesSize / vertexSize;


    std::vector<Vertex> vertices(verticesBuffer, verticesBuffer + numVertices);




    AoVertex* aoVertices = new AoVertex[vertices.size()];
    AoVertex* aoVerticesPointer = aoVertices;







//    LOG_I("aix: %f", fboPixels[0+0] );    LOG_I("pix: %f", fboPixels[0+1] ); LOG_I("pix: %f", fboPixels[0+2] );LOG_I("pix: %f", fboPixels[0+3] );





//    unsigned int* intPixels = (unsigned int*)pixels;

    for (int i=0;i<vertexPosTextureSize;++i){
	for (int j=0;j<vertexPosTextureSize/2;++j){

	    int a =(j * vertexPosTextureSize + i                      )*4;
	    int b =((vertexPosTextureSize-j-1)*vertexPosTextureSize + i)*4;

	    float temp;

	    temp        = pixels[a+0];
	    pixels[a+0] = pixels[b+0];
	    pixels[a+0] = temp;

	    temp        = pixels[a+1];
	    pixels[a+1] = pixels[b+1];
	    pixels[a+1] = temp;

	    temp        = pixels[a+2];
	    pixels[a+2] = pixels[b+2];
	    pixels[a+2] = temp;

	    temp        = pixels[a+3];
	    pixels[a+3] = pixels[b+3];
	    pixels[a+3] = temp;

	}
    }




    // num texels is vertex  count.
  for(int i = 0; i < vertices.size(); ++i) {



      Vertex& v = vertices[i];

	AoVertex aoVertex;

	float ao = pixels[i*4 + 0];

//	if(i < 20)  LOG_I("vert: %f, %f, %f",  pixels[i*4 + 0], pixels[i*4 + 1], pixels[i*4 + 2] );

//	ao = 1;



//	LOG_I("AO: %f", ao);

	aoVertex.point = Vector4f(v.point.x, v.point.y, v.point.z, ao);
	aoVertex.normal = v.normal;
	aoVertex.texCoord = v.texCoord;

	*(aoVerticesPointer++) = aoVertex;

  }




    m_geoObj->m_vertices = ((void *)aoVertices);
    m_geoObj->m_verticesSize = vertices.size() * sizeof(float) * (4+2+3);

    vector<GLuint> vas{4,2,3};
    m_geoObj->m_vertexAttribsSizes = vas;


    return m_geoObj;

    first = false;

}
