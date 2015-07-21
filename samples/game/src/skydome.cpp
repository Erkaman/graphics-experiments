#include "skydome.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include "ewa/value_noise_seed.hpp"
#include "ewa/camera.hpp"

#include "ewa/common.hpp"


using std::vector;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );

Skydome::Skydome(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)), m_delta(0) {

    /*
      Create the skydome.
    */

    MakeSky(radius, slices, stacks);
    MakeSun();

    m_sunTexture = new Texture2D("img/sun.png");

    m_sunTexture->Bind();
    m_sunTexture->SetTextureClamping();
    m_sunTexture->SetMinFilter(GL_LINEAR);
    m_sunTexture->SetMagFilter(GL_NEAREST);
    m_sunTexture->Unbind();

}

void Skydome::MakeSky(const float radius, const int slices, const int stacks) {
    m_domeShader = new ShaderProgram("shader/skydome");

    m_domeVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,3}
	);
    m_domeIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);
    m_domeNumTriangles = GenerateVertices(radius, slices, stacks, m_domeVertexBuffer, m_domeIndexBuffer);

    m_perlinSeed = new ValueNoiseSeed(2);
}

void Skydome::MakeSun() {

    m_sunShader = new ShaderProgram("shader/sun");

    m_sunVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,2}
	);

    m_sunIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    // three points times two triangles
    m_sunNumTriangles = 2;

    FloatVector vertices;
    UshortVector indices;

    Vector3f sunPoint(0, 0, 1);
    Vector3f xaxis(1, 0, 0);
    Vector3f yaxis(0, 1, 0);

    double hsize = 0.06; // 0.3


    (sunPoint - hsize * xaxis + hsize * yaxis).Add(vertices);
    Vector2f(0.0f,1.0f).Add(vertices);

    (sunPoint + hsize * xaxis + hsize * yaxis).Add(vertices);
    Vector2f(1.0f,1.0f).Add(vertices);

    (sunPoint - hsize * xaxis - hsize * yaxis).Add(vertices);
    Vector2f(0.0f,0.0f).Add(vertices);

    (sunPoint + hsize * xaxis - hsize * yaxis).Add(vertices);
    Vector2f(1.0f,0.0f).Add(vertices);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(2);

    m_sunVertexBuffer->Bind();
    m_sunVertexBuffer->SetBufferData(vertices);
    m_sunVertexBuffer->Unbind();

    m_sunIndexBuffer->Bind();
    m_sunIndexBuffer->SetBufferData(indices);
    m_sunIndexBuffer->Unbind();
}

Skydome::~Skydome() {
    MY_DELETE(m_domeShader);
    MY_DELETE(m_domeVertexBuffer);
    MY_DELETE(m_domeIndexBuffer);
    MY_DELETE(m_perlinSeed);

    MY_DELETE(m_sunShader);
    MY_DELETE(m_sunVertexBuffer);
    MY_DELETE(m_sunIndexBuffer);
    MY_DELETE(m_sunTexture);

}

void Skydome::Draw(const Camera& camera) {

    SetDepthTest(false);

    m_domeShader->Bind();

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_domeShader->SetUniform("mvp", mvp);
    m_domeShader->SetUniform("delta", m_delta);

    m_perlinSeed->Bind(*m_domeShader);

    VBO::DrawIndices(*m_domeVertexBuffer, *m_domeIndexBuffer, GL_TRIANGLES, (m_domeNumTriangles)*3);

    m_perlinSeed->Unbind();

    m_domeShader->Unbind();

    const Vector3f sunDir(0,0.5,1);

    const Vector3f taxis(sunDir.z, 0, -sunDir.x);
    const Vector3f yaxis = Vector3f::Cross(sunDir, taxis).Normalize(); //yaxis.cross(taxis);
    const Vector3f xaxis = Vector3f::Cross(yaxis, sunDir).Normalize();

    Matrix4f sunModelView(
	xaxis.x, yaxis.x, sunDir.x, 0,
	xaxis.y, yaxis.y, sunDir.y, 0,
	xaxis.z, yaxis.z, sunDir.z, 0,
	0      , 0      , 0       , 1
	);

    m_sunShader->Bind();

    GL_C(glEnable(GL_BLEND));
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    modelView =  camera.GetModelViewMatrix(sunModelView);
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    mvp = camera.GetProjectionMatrix() * modelView;

    m_sunShader->SetUniform("mvp", mvp);

    m_sunShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_sunTexture->Bind();

    // the texture format is somehow corrupt?

    VBO::DrawIndices(*m_sunVertexBuffer, *m_sunIndexBuffer, GL_TRIANGLES, (m_sunNumTriangles)*3);

    m_sunTexture->Unbind();

    GL_C(glDisable(GL_BLEND));

    m_sunShader->Unbind();

    SetDepthTest(true);
}

void Skydome::Update(const float delta) {
    m_delta += delta;
}
