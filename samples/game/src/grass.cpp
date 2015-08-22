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
#include "ewa/camera.hpp"
#include "height_map.hpp"

#include "ewa/util.hpp"
#include "ewa/random.hpp"
#include "ewa/mult_array.hpp"
#include "ewa/common.hpp"

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

    MY_DELETE(m_billboardVertexBuffer);
    MY_DELETE(m_billboardIndexBuffer);


    MY_DELETE(m_grassShader);
    MY_DELETE(m_grassTexture);

}


Grass::Grass(Vector2f position, HeightMap* heightMap): m_heightMap(heightMap), m_position(position) {

    /*
      Create the skydome.
    */

    m_time = 0;


    m_grassNumTriangles = 0;
    m_billboardNumTriangles = 0;

    m_grassShader = new ShaderProgram("shader/grass");
    m_billboardShader = new ShaderProgram("shader/grass_billboard");

    m_grassTexture = new Texture2D("img/grass_billboard.png");

    m_grassTexture->Bind();
    m_grassTexture->SetTextureClamping();
    m_grassTexture->SetMinFilter(GL_LINEAR);
    m_grassTexture->SetMagFilter(GL_NEAREST);
    m_grassTexture->Unbind();

    m_grassVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,2,3, 3} // pos, texcoord, normal, slot0
	);
    m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_billboardVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,2} // pos, tex
	);
    m_billboardIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    FloatVector grassVertices;
    UshortVector grassIndices;

    FloatVector billboardVertices;
    UshortVector billboardIndices;

    constexpr float SIZE = 0.4f;

    constexpr int COUNT = 20;

    Random rng(12);

    vector<Vector2f> grassPositions;

    Vector2f base(position);

    for(int c = 0; c < COUNT; ++c) {

	Vector2f grassPosition;

	while(true) {

	    grassPosition = base + Vector2f(rng.RandomFloat(-1,+1),rng.RandomFloat(-1,1));

	    bool tooClose = false;

	    for(const Vector2f& p : grassPositions ) {

		float dist = (grassPosition - p).Length();

		//	LOG_I("dist: %f", dist);

		if(dist < SIZE) {
		    tooClose = true;
		    break;
		}
	    }

	    if(!tooClose)
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

    LOG_I("size: %ld", billboardVertices.size());

    m_billboardVertexBuffer->Bind();
    m_billboardVertexBuffer->SetBufferData(billboardVertices);
    m_billboardVertexBuffer->Unbind();

    m_billboardIndexBuffer->Bind();
    m_billboardIndexBuffer->SetBufferData(billboardIndices);
    m_billboardIndexBuffer->Unbind();


}



void Grass::Draw(const Camera& camera, const Vector4f& lightPosition) {

    float cameraDist = ( Vector2f(camera.GetPosition().x, camera.GetPosition().z)  - m_position).Length();

    if(cameraDist < 40) {

	SetCullFace(false);

	m_grassShader->Bind();
	m_grassShader->SetUniform("time", m_time);

//	GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
//	GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// draw grass.

	const Matrix4f model =  Matrix4f::CreateIdentity();

	m_grassShader->SetPhongUniforms(model, camera, lightPosition);

	m_grassShader->SetUniform("tex", 0);
	Texture::SetActiveTextureUnit(0);
	m_grassTexture->Bind();


	VBO::DrawIndices(*m_grassVertexBuffer, *m_grassIndexBuffer, GL_TRIANGLES, (m_grassNumTriangles)*3);

	m_grassTexture->Unbind();

	// done drawing billboards.
//	GL_C(glDisable(GL_BLEND));
	m_grassShader->Unbind();

	SetCullFace(true);
    } else {

	SetCullFace(false);


	m_billboardShader->Bind();

//	GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
//	GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// draw grass.

	const Matrix4f model =  Matrix4f::CreateIdentity();


	const Matrix4f view = camera.GetViewMatrix();


//	const Matrix4f mvp = camera.GetMvp(modelViewMatrix);
	m_billboardShader->SetUniform("model", model);
	m_billboardShader->SetUniform("view", view);

	m_billboardShader->SetUniform("projection", camera.GetProjectionMatrix());
	m_billboardShader->SetUniform("offset", m_position);


	m_billboardShader->SetUniform("tex", 0);
	Texture::SetActiveTextureUnit(0);
	m_grassTexture->Bind();



	VBO::DrawIndices(*m_billboardVertexBuffer, *m_billboardIndexBuffer, GL_TRIANGLES, (m_billboardNumTriangles)*3);

	m_grassTexture->Unbind();


	SetCullFace(true);



	// done drawing billboards.
//	GL_C(glDisable(GL_BLEND));
	m_billboardShader->Unbind();
    }
}

void Grass::Update(const float delta) {
    m_time += delta;

//    LOG_I("time: %f",m_time);
}

void Grass::GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height) {
    GLushort baseIndex = grassVertices.size() / (3+2+3+3);

    Vector2f dir = AngleToVector(angle);
    Vector3f normal(0,1,0);
    Vector3f centerPosition(position.x, m_heightMap->GetHeightAt(position.x, position.y)/* - 0.09f*/ ,position.y);
    dir.Normalize();

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    (centerPosition+Vector3f(-X, height, -Z)).Add(grassVertices);
    Vector2f(0.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (centerPosition+Vector3f(+X, height, +Z)).Add(grassVertices);
    Vector2f(1.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (centerPosition+Vector3f(-X, 0, -Z)).Add(grassVertices);
    Vector2f(0.0f,1.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (centerPosition+Vector3f(+X, 0, +Z)).Add(grassVertices);
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

    GenerateBillboardVertices(position - m_position,billboardVertices, billboardIndices, width,height);
}

void Grass::GenerateBillboardVertices(const Vector2f position, FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height) {

    GLushort baseIndex = billboardVertices.size() / (3+2);


    Vector2f dir = AngleToVector(0);
    Vector3f centerPosition(position.x, m_heightMap->GetHeightAt(position.x+m_position.x, position.y+m_position.y) ,position.y);
    dir.Normalize();

/*
  INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.613633
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.159506
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.042944
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.449350
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.136868
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.059547
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.868878
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.815653
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.217096
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.857285
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.780457
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.833551
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.006429
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.529883
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.411310
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.570762
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.964326
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.345138
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 10.981901
INFO: /Users/eric/tuhu/samples/game/src/grass.cpp:311:GenerateBillboardVertices:pos: 9.456833
 */

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    LOG_I("pos: %f", position.x);


    (centerPosition+Vector3f(-X, height, -Z)).Add(billboardVertices);
    Vector2f(0.0f,0.0f).Add(billboardVertices);

    (centerPosition+Vector3f(+X, height, +Z)).Add(billboardVertices);
    Vector2f(1.0f,0.0f).Add(billboardVertices);

    (centerPosition+Vector3f(-X, 0, -Z)).Add(billboardVertices);
    Vector2f(0.0f,1.0f).Add(billboardVertices);

    (centerPosition+Vector3f(+X, 0, +Z)).Add(billboardVertices);
    Vector2f(1.0f,1.0f).Add(billboardVertices);

    billboardIndices.push_back(baseIndex+0);
    billboardIndices.push_back(baseIndex+1);
    billboardIndices.push_back(baseIndex+2);

    billboardIndices.push_back(baseIndex+1);
    billboardIndices.push_back(baseIndex+3);
    billboardIndices.push_back(baseIndex+2);

    m_billboardNumTriangles += 2;
}
