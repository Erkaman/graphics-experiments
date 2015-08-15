#include "grass.hpp"

#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/random.hpp"

#include <algorithm>

using std::vector;
using std::string;

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

    m_grassTexture = new Texture2D("img/grass.png");

    m_grassTexture->Bind();
    m_grassTexture->SetTextureClamping();
    m_grassTexture->SetMinFilter(GL_LINEAR);
    m_grassTexture->SetMagFilter(GL_NEAREST);
    m_grassTexture->Unbind();

    MakeGrass();

}

void Grass::Draw(const Camera& camera, const Vector4f& lightPosition) {


    //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    SetCullFace(false);

    m_shader->Bind();

    m_shader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();

    m_shader->SetPhongUniforms(

	Matrix4f::CreateTranslation(7,4,5)
	, camera, lightPosition);

    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));



    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    GL_C(glDisable(GL_BLEND));


    m_grassTexture->Unbind();


    m_shader->Unbind();

    //   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


    SetCullFace(true);

}
Grass::~Grass() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_shader);
    MY_DELETE(m_grassTexture);
}


void Grass::MakeGrassBlade(FloatVector& vertices, UshortVector& indices,
			   const Vector3f& vertexPosition,
			   const int lod,
			   const float grassHeight, const float grassWidth,
			   const Vector3f& windDirection

    ) {

    Vector3f startPoint, endPoint, startTopPoint, endTopPoint, normal;

    Vector3f vertexNormal(0,1,0);


    startPoint = vertexPosition;
    startPoint.x = vertexPosition.x - 2.0f*grassWidth/4.0f;
    endPoint = startPoint;
    endPoint.x += 2.0f * grassWidth/2.0f;

    // http://outerra.blogspot.se/2012/05/procedural-grass-rendering.html
    // http://stijndelaruelle.com/pdf/grass.pdf
    // file:///Users/eric/Dropbox/LeeRealtimeGrassThesis.pdf
    for (int i = 0; i < lod; ++i) {
	startTopPoint = startPoint + (Vector3f(grassHeight/lod, grassHeight/lod, grassHeight/lod) *
				      vertexNormal);
	endTopPoint = endPoint + (Vector3f(grassHeight/lod, grassHeight/lod, grassHeight/lod) *
				  vertexNormal);


	float windCoeff = (float)(i+1) / (float)lod;

	startTopPoint.x += windDirection.x* windCoeff;
	startTopPoint.z += windDirection.z* windCoeff;

	endTopPoint.x += windDirection.x * windCoeff;
	endTopPoint.z += windDirection.z * windCoeff;

	Vector3f edgeDirection = startTopPoint - startPoint;
	float edgeLength = edgeDirection.Length();
	float deltaY = edgeLength - (grassHeight/(float)lod);

	edgeDirection.Normalize(); //normalize here as we use it to calculate the lenght above

	startTopPoint -= edgeDirection * deltaY;
	endTopPoint -= edgeDirection * deltaY;

	normal = vertexNormal;

	float bottomY = 1.0f - (i * (1.0f/lod));

	GLushort baseIndex = vertices.size() / (3+3+2);

	startPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(0.0f,bottomY).Add(vertices);

	endPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(1.0f,bottomY).Add(vertices);

	startTopPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(0.0f,bottomY - (1.0f/lod)).Add(vertices);

	endTopPoint.Add(vertices);
	normal.Add(vertices);
	Vector2f(1.0f,bottomY - (1.0f/lod)).Add(vertices);


	indices.push_back(baseIndex+0);
	indices.push_back(baseIndex+1);
	indices.push_back(baseIndex+2);

	indices.push_back(baseIndex+3);
	indices.push_back(baseIndex+2);
	indices.push_back(baseIndex+1);

	startPoint = startTopPoint;
	endPoint = endTopPoint;

	m_numTriangles += 2;
    }
}

void Grass::MakeGrass() {
    FloatVector vertices;
    UshortVector indices;
    m_numTriangles = 0;

    constexpr int LOD =5;
    constexpr int BLADES =30;

    const float width = 0.10f;

    Random rng(10);

//	;->RandomInt(0, NUM_CLOUD_TEXTURES-1)];

    struct GrassBlade {
	Vector3f vertexPosition;
	float grassHeight;
	float grassWidth;
	Vector3f windDirection;
    };

    vector<GrassBlade> blades;

    for(int i = 0; i < BLADES; ++i) {

	GrassBlade blade;

	blade.vertexPosition = Vector3f( rng.RandomFloat(-0.5f, +0.5f),0,rng.RandomFloat(-0.3f, +0.3f));

	blade.grassHeight = rng.RandomFloat(0.8f, 1.2f);
	blade.grassWidth = rng.RandomFloat(0.08f, 0.12f);
	blade.windDirection = Vector3f(rng.RandomFloat(-0.15f, +0.15f),0,0.0f);

	blades.push_back(blade);
    }

    std::sort(blades.begin(), blades.end(),
	      [](const GrassBlade& a, const GrassBlade& b) -> bool{
		  return a.vertexPosition.z > b.vertexPosition.z;
	      });

    for(const GrassBlade& blade : blades) {
	MakeGrassBlade(vertices, indices, blade.vertexPosition,
		       LOD, blade.grassHeight, blade.grassWidth, blade.windDirection);

    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();
}
