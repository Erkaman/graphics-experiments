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



    MY_DELETE(m_grassTexture);
}


Grass::Grass(Vector2f position, HeightMap* heightMap): m_rng(12), m_heightMap(heightMap), m_position(position) {

    m_currentId = 0;

    /*
      Create the skydome.
    */

    m_time = 0;

    m_grassNumTriangles = 0;

    vector<string> defaultDefines = m_heightMap->GetDefaultDefines();
    string shaderName = "shader/grass";

    {
	vector<string> defines(defaultDefines);
//	defines.push_back("SHADOW_MAPPING");


	defines.push_back("DEFERRED");

	m_deferredShader =
	    ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }



    {
	vector<string> defines(defaultDefines);

	m_reflectionShader =
	    ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }


    {
	vector<string> defines(defaultDefines);
	defines.push_back("OUTPUT_ID");

	m_outputIdShader =
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
	vector<GLuint>{3,2,3, 3} // pos, texcoord, normal, slot0
	);
    m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);



}

void Grass::Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader) {

    SetCullFace(false);

    shader->Bind();
    shader->SetUniform("time", m_time);

    const Matrix4f model =  Matrix4f::CreateIdentity();

    shader->SetPhongUniforms(model, camera, lightPosition, Matrix4f::CreateIdentity() );

    shader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    shader->SetUniform("heightMap", 1);
    Texture::SetActiveTextureUnit(1);
    m_heightMap->GetHeightMap()->Bind();

    VBO::DrawIndices(*m_grassVertexBuffer, *m_grassIndexBuffer, GL_TRIANGLES, (m_grassNumTriangles)*3);

    m_grassTexture->Unbind();

    shader->Unbind();

    SetCullFace(true);

}

void Grass::DrawDeferred(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_deferredShader);
}

void Grass::DrawReflection(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_reflectionShader);
}

void Grass::Update(const float delta) {
    m_time += delta;
}

void Grass::GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height, int id) {
    GLushort baseIndex = (GLushort)grassVertices.size() / (3+2+3+3);

    Vector2f dir = AngleToVector(angle);
    Vector3f normal(0,1,0);

    Vector3f centerPosition(position.x ,position.y, id);

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

void Grass::MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height, int id) {
    GenerateGrassVertices(position, 0+angle,grassVertices, grassIndices, width,height, id);
    GenerateGrassVertices(position, 60+angle,grassVertices, grassIndices, width,height, id);
    GenerateGrassVertices(position, 120+angle,grassVertices, grassIndices, width,height, id);

//    GenerateBillboardVertices(position - m_position,billboardVertices, billboardIndices, width,height);
}


void Grass::Rebuild() {

    FloatVector grassVertices;
    UshortVector grassIndices;
    m_grassNumTriangles = 0;

      FloatVector billboardVertices;
      UshortVector billboardIndices;

      constexpr float SPREAD = 10.0f;


    /*


      constexpr int COUNT = 50;



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
    */


    for(auto& pair : m_grass) {

	GrassInfo grass = pair.second;
	int id = pair.first;

      MakeGrass(grass.pos, grass.angle , grassVertices, grassIndices, billboardVertices,
		billboardIndices, grass.size, grass.size,id);
    }


    m_grassVertexBuffer->Bind();
    m_grassVertexBuffer->SetBufferData(grassVertices);
    m_grassVertexBuffer->Unbind();

    m_grassIndexBuffer->Bind();
    m_grassIndexBuffer->SetBufferData(grassIndices);
    m_grassIndexBuffer->Unbind();
}

void Grass::AddGrass(const Vector2i& position, int grassClusterSize) {

    int numGrass = (grassClusterSize-1) * m_rng.RandomFloat(+4,+6);
    int dist = (grassClusterSize-1) * 3.5;

    if(numGrass <= 0)
	numGrass = 1;
    if(dist <= 0.01) {
	dist = 0;
    }


    for(int i = 0; i < numGrass; ++i ) {

	GrassInfo grass;

	grass.pos =  Vector2f(
	    (float)position.x + m_rng.RandomFloat(-dist,+dist) + m_rng.RandomFloat(-1,+1),
	    (float)position.y + m_rng.RandomFloat(-dist,+dist) + m_rng.RandomFloat(-1,+1));
	grass.angle = m_rng.RandomFloat(-90,+90);
	grass.size =m_rng.RandomFloat(2.0f,2.7f);

	m_grass[m_currentId++] = grass;

    }

    Rebuild();
}


void Grass::RemoveGrass(int id) {
    m_grass.erase(id);
    Rebuild();
}


void Grass::RenderIdAll(const ICamera* camera) {
    Draw(camera, Vector4f(0), m_outputIdShader );
}
