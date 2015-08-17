#include "grass.hpp"
#include "biased_random.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include "ewa/value_noise_seed.hpp"
#include "ewa/camera.hpp"

#include "ewa/util.hpp"
#include "ewa/random.hpp"
#include "ewa/mult_array.hpp"

using std::string;
using std::vector;


Grass::Grass() {

    /*
      Create the skydome.
    */


    m_grassShader = new ShaderProgram("shader/billboard");



    m_grassTexture = new Texture2D("img/grass_billboard.png");

    m_grassTexture->Bind();
    m_grassTexture->SetTextureClamping();
    m_grassTexture->SetMinFilter(GL_LINEAR);
    m_grassTexture->SetMagFilter(GL_NEAREST);
    m_grassTexture->Unbind();



    m_grassVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,2}
	);
    m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    FloatVector vertices;
    UshortVector indices;

    constexpr float SIZE = 0.4f;

    GenerateBillboardVertices(Vector2f(1,0),vertices, indices, SIZE,SIZE);
    GenerateBillboardVertices(Vector2f(0,1),vertices, indices, SIZE,SIZE);


    m_grassVertexBuffer->Bind();
    m_grassVertexBuffer->SetBufferData(vertices);
    m_grassVertexBuffer->Unbind();

    m_grassIndexBuffer->Bind();
    m_grassIndexBuffer->SetBufferData(indices);
    m_grassIndexBuffer->Unbind();

}

void Grass::Draw(const Camera& camera) {
    // next we'll draw all the billboards:

    SetCullFace(false);


    m_grassShader->Bind();
    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // draw grass.

    const Matrix4f model =  Matrix4f::CreateTranslation(10,-4.0,10) * Matrix4f::CreateRotate(90, Vector3f(0,1,0));
    Matrix4f modelView =  camera.GetModelViewMatrix(model);
    const Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_grassShader->SetUniform("mvp", mvp);

    m_grassShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    VBO::DrawIndices(*m_grassVertexBuffer, *m_grassIndexBuffer, GL_TRIANGLES, (m_grassNumTriangles)*3);

    m_grassTexture->Unbind();


    // done drawing billboards.
    GL_C(glDisable(GL_BLEND));
    m_grassShader->Unbind();

    SetCullFace(true);
}

void Grass::Update(const float delta) {
}

void Grass::GenerateBillboardVertices(const Vector2f& dir, FloatVector& vertices, UshortVector& indices, const float width, const float height) {
    GLushort baseIndex = vertices.size() / (3+2);

    //   Vector2f dir(1,0);
    dir.Normalize();

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    Vector3f(-X, height, -Z).Add(vertices);
    Vector2f(0.0f,0.0f).Add(vertices);

    Vector3f(+X, height, +Z).Add(vertices);
    Vector2f(1.0f,0.0f).Add(vertices);

    Vector3f(-X, 0, -Z).Add(vertices);
    Vector2f(0.0f,1.0f).Add(vertices);

    Vector3f(+X, 0, +Z).Add(vertices);
    Vector2f(1.0f,1.0f).Add(vertices);

    indices.push_back(baseIndex+0);
    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+2);

    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+3);
    indices.push_back(baseIndex+2);

    m_grassNumTriangles += 2;
}
