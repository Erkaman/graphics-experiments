#include "grass.hpp"
#include "biased_random.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/math/math_common.hpp"

#include "ewa/value_noise_seed.hpp"
#include "ewa/icamera.hpp"
#include "height_map.hpp"

#include "ewa/util.hpp"
#include "ewa/random.hpp"
#include "ewa/mult_array.hpp"
#include "ewa/common.hpp"
#include "resource_manager.hpp"

using std::string;
using std::vector;

Vector2f AngleToVector(const float angle) {
    const float radians = ToRadians(angle);
    return Vector2f(
	cos(radians),
	sin(radians)
	);
}

Grass::~Grass() {
    MY_DELETE(m_grassVertexBuffer);
    MY_DELETE(m_grassIndexBuffer);



    MY_DELETE(m_grassShader);
    MY_DELETE(m_grassTexture);
}


Grass::Grass(Vector2f position, HeightMap* heightMap): m_heightMap(heightMap), m_position(position) {

    /*
      Create the skydome.
    */

    m_time = 0;

    m_grassNumTriangles = 0;

    vector<string> defaultDefines = m_heightMap->GetDefaultDefines();
    {
	vector<string> defines(defaultDefines);
/*	defines.push_back("SHADOW_MAPPING");
	defines.push_back("DEFERRED");
*/

	string shaderName = "shader/grass";

	m_grassShader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }

    m_grassTexture = Texture2D::Load("img/grass_billboard.png");

    if(!m_grassTexture) {
	PrintErrorExit();
    }

    m_grassTexture->Bind();
    m_grassTexture->SetTextureClamping();
    m_grassTexture->SetMinFilter(GL_LINEAR);
    m_grassTexture->SetMagFilter(GL_NEAREST);
    m_grassTexture->Unbind();

    m_grassVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,2,3, 2} // pos, texcoord, normal, slot0
	);
    m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    FloatVector grassVertices;
    UshortVector grassIndices;

    FloatVector billboardVertices;
    UshortVector billboardIndices;

    constexpr float SIZE = 2.5f;
    constexpr float SPREAD = 10.0f;

    constexpr int COUNT = 50;

    Random rng(12);

    vector<Vector2f> grassPositions;

    Vector2f base(Vector2f(175,175));

    for(int c = 0; c < COUNT; ++c) {

	Vector2f grassPosition;

	while(true) {

	    grassPosition = base + Vector2f(rng.RandomFloat(-SPREAD,+SPREAD),rng.RandomFloat(-SPREAD,SPREAD));
	    break;
	}

	MakeGrass(grassPosition, rng.RandomFloat(-90,+90), grassVertices, grassIndices, billboardVertices, billboardIndices, SIZE,SIZE);

	grassPositions.push_back(grassPosition);
    }

    base = (Vector2f(250,385));

    for(int c = 0; c < COUNT; ++c) {

	Vector2f grassPosition;

	while(true) {

	    grassPosition = base + Vector2f(rng.RandomFloat(-SPREAD,+SPREAD),rng.RandomFloat(-SPREAD,SPREAD));
	    break;
	}

	MakeGrass(grassPosition, rng.RandomFloat(-90,+90), grassVertices, grassIndices, billboardVertices, billboardIndices, SIZE,SIZE);

	grassPositions.push_back(grassPosition);
    }


    m_grassVertexBuffer->Bind();
    m_grassVertexBuffer->SetBufferData(grassVertices);
    m_grassVertexBuffer->Unbind();

    m_grassIndexBuffer->Bind();
    m_grassIndexBuffer->SetBufferData(grassIndices);
    m_grassIndexBuffer->Unbind();

}

void Grass::Draw(const ICamera* camera, const Vector4f& lightPosition) {

    SetCullFace(false);

    m_grassShader->Bind();
    m_grassShader->SetUniform("time", m_time);

    const Matrix4f model =  Matrix4f::CreateIdentity();

    m_grassShader->SetPhongUniforms(model, camera, lightPosition, Matrix4f::CreateIdentity() );

    m_grassShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    m_grassShader->SetUniform("heightMap", 1);
    Texture::SetActiveTextureUnit(1);
    m_heightMap->GetHeightMap()->Bind();



    VBO::DrawIndices(*m_grassVertexBuffer, *m_grassIndexBuffer, GL_TRIANGLES, (m_grassNumTriangles)*3);

    m_grassTexture->Unbind();

    m_grassShader->Unbind();

    SetCullFace(true);
}

void Grass::Update(const float delta) {
    m_time += delta;

//    LOG_I("time: %f",m_time);
}

void Grass::GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height) {
    GLushort baseIndex = (GLushort)grassVertices.size() / (3+2+3+2);

    Vector2f dir = AngleToVector(angle);
    Vector3f normal(0,1,0);
//    Vector3f centerPosition(position.x, m_heightMap->GetHeightAt(position.x, position.y)/* - 0.09f*/ ,position.y);

    Vector2f centerPosition(position.x ,position.y);

//    Vector3f centerPosition(-63.863667, 15.000000, -78.305321);

//    LOG_I("center: %s", string(centerPosition).c_str() );
//    Vector3f centerPosition(0,15,0);



    dir.Normalize();

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    (Vector3f(-X, height, -Z)).Add(grassVertices);
    Vector2f(0.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, height, +Z)).Add(grassVertices);
    Vector2f(1.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(-X, 0, -Z)).Add(grassVertices);
    Vector2f(0.0f,1.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, 0, +Z)).Add(grassVertices);
    Vector2f(1.0f,1.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    grassIndices.push_back(baseIndex+0);
    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+2);

    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+3);
    grassIndices.push_back(baseIndex+2);

    m_grassNumTriangles += 2;
}

void Grass::MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height) {
    GenerateGrassVertices(position, 0+angle,grassVertices, grassIndices, width,height);
    GenerateGrassVertices(position, 60+angle,grassVertices, grassIndices, width,height);
    GenerateGrassVertices(position, 120+angle,grassVertices, grassIndices, width,height);

//    GenerateBillboardVertices(position - m_position,billboardVertices, billboardIndices, width,height);
}
