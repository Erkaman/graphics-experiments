#include "skybox.hpp"

#include "ewa/camera.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"

static void  AddFace(
    FloatVector& positions,
    UshortVector& indices,
    const float ax, const float ay, const float az,
    const float bx, const float by, const float bz,
    const float cx, const float cy, const float cz,
    const float dx, const float dy, const float dz
    ) {
    const GLushort base = (GLushort)(positions.size() / 3);

    positions.push_back(ax);
    positions.push_back(ay);
    positions.push_back(az);

    positions.push_back(bx);
    positions.push_back(by);
    positions.push_back(bz);

    positions.push_back(cx);
    positions.push_back(cy);
    positions.push_back(cz);

    positions.push_back(dx);
    positions.push_back(dy);
    positions.push_back(dz);

    indices.push_back(base);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
}

Skybox::Skybox(const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
	       const std::string& rightFace, const std::string& upFace, const std::string& downFace): m_cubeMap(
		   frontFace, backFace, leftFace,
		   rightFace, upFace, downFace
		   ) {


    // load shader.
    m_shader = new ShaderProgram("shader/skybox");


    // create the vertex data.

    FloatVector positions;
    positions.reserve(4*3*6);

    UshortVector indices;
    indices.reserve(6*6);

    // create the vertices of a unit cube.

    AddFace(positions, indices,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f);

    //Back face
    AddFace(positions, indices,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f);

    //Top face
    AddFace(positions, indices,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f
	);

    //Bottom face
    AddFace(positions, indices,
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f
	);

    //Right face
    AddFace(positions, indices,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f
	);

    //Left face
    AddFace(positions, indices,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f);

    m_numIndices =(GLushort)indices.size();

    m_positionBuffer = VBO::CreatePosition(3);

    m_positionBuffer->Bind();
    m_positionBuffer->SetBufferData(positions);
    m_positionBuffer->Unbind();

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();


}

void Skybox::Draw(const Camera& camera) {
    SetupForRender();

    SetDepthTest(false);

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
    m_shader->SetUniform("sampler", 0);


    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);


    SetDepthTest(true);

    UnsetupForRender();
}

void Skybox::SetupForRender() {
    m_shader->Bind();

    m_positionBuffer->EnableVertexAttribWithBind();

    m_indexBuffer->Bind();

    Texture::SetActiveTextureUnit(0);

    m_cubeMap.Bind();
}

void Skybox::UnsetupForRender() {

    m_positionBuffer->DisableVertexAttribWithBind();

    m_indexBuffer->Unbind();

    m_cubeMap.Unbind();

    m_shader->Unbind();
}

Skybox::~Skybox() {
    delete m_indexBuffer;
    delete m_positionBuffer;
    delete m_shader;
}
