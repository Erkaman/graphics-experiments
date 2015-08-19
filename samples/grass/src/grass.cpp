#include "grass.hpp"

#include "ewa/common.hpp"
#include "ewa/keyboard_state.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

#include <algorithm>

using std::vector;
using std::string;

void Grass::AddQuad(GrassGroup* grassGroup,
		    const Vector3f& bottomLeft, const Vector3f& bottomRight,
		    const Vector3f& topLeft, const Vector3f& topRight) {

    const Vector3f n = Vector3f::Cross(topRight - bottomRight,bottomRight - bottomLeft).Normalize();

    // LOG_I("normal %s", tos(n).c_str());


    FloatVector& vertices = grassGroup->m_vertices;
    UshortVector indices = grassGroup->m_indices;

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

    grassGroup->m_numTriangles += 2;
}

Grass::Grass(const std::vector<std::string>& textureFilenames, const Vector4f& lightPosition): NUM_GRASS_GROUPS(textureFilenames.size()), m_lightPosition(lightPosition){



    m_shader = new ShaderProgram("shader/grass");


    for(int i = 0; i < NUM_GRASS_GROUPS; ++i) {

	GrassGroup* grassGroup = new GrassGroup();

	grassGroup->m_vertexBuffer = VBO::CreateInterleaved(
	    vector<GLuint>{
		VBO_POSITION_ATTRIB_INDEX,
		    VBO_NORMAL_ATTRIB_INDEX,
		    VBO_TEX_COORD_ATTRIB_INDEX},
	    vector<GLuint>{3,3,2}
	    );


	grassGroup->m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

	grassGroup->m_numTriangles = 0;





	Texture* grassTexture = new Texture2D(textureFilenames[i]);


	grassTexture->Bind();
	grassTexture->SetTextureClamping();
	grassTexture->SetMinFilter(GL_LINEAR);
	grassTexture->SetMagFilter(GL_NEAREST);
	grassTexture->Unbind();

	grassGroup->m_grassTexture = grassTexture;


	m_grassGroups.push_back(grassGroup);
    }

    LOG_I("done constructor");
}

void Grass::Init() {

    MakeGrass();


    LOG_I("done init");
}

void Grass::Draw(const Camera& camera) {


    //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    SetCullFace(false);

    m_shader->Bind();

    m_shader->SetPhongUniforms(

	Matrix4f::CreateTranslation(7,4,5)
	, camera, m_lightPosition);

    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));




    for(int i = 0; i < NUM_GRASS_GROUPS; ++i) {

	GrassGroup* grassGroup = m_grassGroups[i];

	m_shader->SetUniform("grass", 0);
	Texture::SetActiveTextureUnit(0);
	grassGroup->m_grassTexture->Bind();

	VBO::DrawIndices(*grassGroup->m_vertexBuffer, *grassGroup->m_indexBuffer, GL_TRIANGLES, (grassGroup->m_numTriangles)*3);
	grassGroup->m_grassTexture->Unbind();


    }



    GL_C(glDisable(GL_BLEND));

    m_shader->Unbind();

    //   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


    SetCullFace(true);

}
Grass::~Grass() {
    for(GrassGroup* grassGroup : m_grassGroups) {
	MY_DELETE(grassGroup);
    }

    MY_DELETE(m_shader);
}


void Grass::MakeGrassBlade(GrassGroup* grassGroup,
			   const Vector3f& vertexPosition,
			   const int lod,
			   const float grassHeight, const float grassWidth,
			   const Vector3f& windDirection

    ) {

    FloatVector& vertices = grassGroup->m_vertices;
    UshortVector& indices = grassGroup->m_indices;



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

	Vector3f n(0,0,-1);

//	const Vector3f n = Vector3f::Cross(endPoint - startPoint,startTopPoint - startPoint).Normalize();


	startPoint.Add(vertices);
	n.Add(vertices);
	Vector2f(0.0f,bottomY).Add(vertices);

	endPoint.Add(vertices);
	n.Add(vertices);
	Vector2f(1.0f,bottomY).Add(vertices);

	startTopPoint.Add(vertices);
	n.Add(vertices);
	Vector2f(0.0f,bottomY - (1.0f/lod)).Add(vertices);

	endTopPoint.Add(vertices);
	n.Add(vertices);
	Vector2f(1.0f,bottomY - (1.0f/lod)).Add(vertices);

	indices.push_back(baseIndex+0);
	indices.push_back(baseIndex+1);
	indices.push_back(baseIndex+2);

	indices.push_back(baseIndex+3);
	indices.push_back(baseIndex+2);
	indices.push_back(baseIndex+1);

	startPoint = startTopPoint;
	endPoint = endTopPoint;

	grassGroup->m_numTriangles += 2;
    }
}


void Grass::Update(const float delta) {
    const KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_Y) ) {
	m_lightPosition.z += 0.01f;
    }
    if( kbs.IsPressed(GLFW_KEY_H) ) {
	m_lightPosition.z -= 0.01f;
    }

}

void Grass::MakeGrass() {
/*    FloatVector vertices;
      UshortVector indices;
      m_numTriangles = 0;*/


    LOG_I("makegrass1");

    std::vector<GrassBlade> blades;

    MakeGrass(blades);

    LOG_I("makegrass2");



    std::sort(blades.begin(), blades.end(),
	      [](const GrassBlade& a, const GrassBlade& b) -> bool{
		  return a.vertexPosition.z > b.vertexPosition.z;
	      });

    for(const GrassBlade& blade : blades) {
	MakeGrassBlade(m_grassGroups[blade.grassTexture], blade.vertexPosition,
		       blade.lod, blade.grassHeight, blade.grassWidth, blade.windDirection);

    }



    for(int i = 0; i < NUM_GRASS_GROUPS; ++i) {

	GrassGroup* grassGroup = m_grassGroups[i];

	VBO* vertexBuffer = grassGroup->m_vertexBuffer;
	VBO* indexBuffer = grassGroup->m_indexBuffer;

	vertexBuffer->Bind();
	vertexBuffer->SetBufferData(grassGroup->m_vertices);
	vertexBuffer->Unbind();

	indexBuffer->Bind();
	indexBuffer->SetBufferData(grassGroup->m_indices);
	indexBuffer->Unbind();
    }

    LOG_I("makegrass4");

}

GrassGroup::~GrassGroup() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_grassTexture);
}
