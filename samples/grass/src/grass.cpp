#include "grass.hpp"

#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

using std::vector;

void Grass::AddQuad(FloatVector& vertices, UshortVector& indices,
		    const Vector3f& bottomLeft, const Vector3f& bottomRight,
		    const Vector3f& topLeft, const Vector3f& topRight) {

    const Vector3f n = Vector3f::Cross(topRight - bottomRight,bottomRight - bottomLeft).Normalize();

    // LOG_I("normal %s", tos(n).c_str());

    bottomLeft.Add(vertices);
    n.Add(vertices);

    bottomRight.Add(vertices);
    n.Add(vertices);

    topLeft.Add(vertices);
    n.Add(vertices);

    topRight.Add(vertices);
    n.Add(vertices);


    GLushort baseIndex = indices.size() / 3;

    indices.push_back(baseIndex+0);
    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+2);

    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+2);
    indices.push_back(baseIndex+3);

    m_numTriangles += 2;

}


Grass::Grass( ){

    m_shader = new ShaderProgram("shader/grass");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_NORMAL_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX},
	vector<GLuint>{3,3,2}
	);


    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    MakeGrass();

}

void Grass::Draw(const Camera& camera, const Vector4f& lightPosition) {


    //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    SetCullFace(false);

    m_shader->Bind();

    m_shader->SetPhongUniforms(

	Matrix4f::CreateIdentity()
	, camera, lightPosition);


    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_shader->Unbind();

    // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


    SetCullFace(true);

}
Grass::~Grass() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_shader);
}

void Grass::MakeGrass() {
    FloatVector vertices;
    UshortVector indices;
    m_numTriangles = 0;

    Vector3f vertexNormal(0,1,0);
    Vector3f vertexPosition(0,0,0);

    if (Vector3f::Dot(vertexNormal, Vector3f(0, 1, 0)) < 0) return;

    Vector3f startPoint, endPoint, startTopPoint, endTopPoint, normal;

    Vector3f normalPos = Vector3f::Normalize(vertexPosition);
    float grassHeight = 5;

    Vector3f windDirection = Vector3f(0.2f); //random offset

    float m_GrassWidth = 4;

    startPoint = vertexPosition;
    startPoint.x = vertexPosition.x - m_GrassWidth/4;
    endPoint = startPoint;
    endPoint.x += m_GrassWidth/2;

    int lod = 1;
    constexpr int MAX_LOD = 10;


    // http://outerra.blogspot.se/2012/05/procedural-grass-rendering.html
    // http://stijndelaruelle.com/pdf/grass.pdf
    // file:///Users/eric/Dropbox/LeeRealtimeGrassThesis.pdf
    for (int i = 0; i < lod; ++i)
    {
	startTopPoint = startPoint + (Vector3f(grassHeight/lod, grassHeight/lod, grassHeight/lod) *
				      vertexNormal);
	endTopPoint = endPoint + (Vector3f(grassHeight/lod, grassHeight/lod, grassHeight/lod) *
				  vertexNormal);

	Vector3f windDirection(0.2f, 0.2f, 0.2f);

	startTopPoint.x += windDirection.x;
	startTopPoint.z += windDirection.z;

	endTopPoint.x += windDirection.x;
	endTopPoint.z += windDirection.z;


	Vector3f edgeDirection = startTopPoint - startPoint;
	float edgeLength = edgeDirection.Length();
	float deltaY = edgeLength - (grassHeight/lod);
	edgeDirection.Normalize(); //normalize here as we use it to calculate the lenght above

	startTopPoint -= edgeDirection * deltaY;
	endTopPoint -= edgeDirection * deltaY;

	normal = vertexNormal;

	float bottomY = 1.0f - (i * (1/lod));

	startPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(0.0f,bottomY).Add(vertices);


	endPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(1.0f,bottomY).Add(vertices);


	startTopPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(0.0f,bottomY - (1/lod)).Add(vertices);

	endTopPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(1.0f,bottomY - (1/lod)).Add(vertices);


	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(0);


	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);



	/*
	  CreateVertex(triStream,
	  startPoint, // pos
	  normal, // normal
	  float2(0.0f, bottomY), // texture coordinate.
	  vertex[0].Position);

	  CreateVertex(triStream, endPoint, normal, float2(1.0f, bottomY), vertex[0].Position);

	  CreateVertex(triStream, startTopPoint, normal, float2(0.0f, bottomY - (1/lod)),
	  vertex[0].Position);

	  CreateVertex(triStream, endTopPoint, normal, float2(1.0f, bottomY - (1/lod)),
	  vertex[0].Position);

	*/

	startPoint = startTopPoint;
	endPoint = endTopPoint;

    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();
}
