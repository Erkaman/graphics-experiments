#include "skydome.hpp"

#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"
#include "gl/shader_program.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/matrix4f.hpp"

#include "value_noise_seed.hpp"
#include "camera.hpp"

#include "common.hpp"


using std::vector;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );

Skydome::Skydome(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)), m_delta(0) {

    /*
      load the shader
    */

    m_shader = new ShaderProgram("shader/sphere");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,3}
	);

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_numTriangles = GenerateVertices(radius, slices, stacks, m_vertexBuffer, m_indexBuffer);

    m_perlinSeed = new ValueNoiseSeed(2);
}



Skydome::~Skydome() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_perlinSeed);

}

void Skydome::Draw(const Camera& camera) {

    m_shader->Bind();

    SetDepthTest(false);

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
//    m_shader->SetUniform("sampler", 0);
    m_shader->SetUniform("delta", m_delta);

    m_perlinSeed->Bind(*m_shader);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_perlinSeed->Unbind();


    m_shader->Unbind();

    SetDepthTest(true);

}

void Skydome::Update(const float delta) {
    m_delta += delta;
}
