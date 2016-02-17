#include "skybox.hpp"

#include "ewa/camera.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"

using std::vector;

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

Skybox::Skybox() {


    // load shader.
    m_shader = ShaderProgram::Load("shader/skybox");


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

    m_positionBuffer = VBO::CreateInterleaved(
	    vector<GLuint>{3} // pos,
	    );

    m_positionBuffer->Bind();
    m_positionBuffer->SetBufferData(positions);
    m_positionBuffer->Unbind();

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();


}

void Skybox::Draw(CubeMapTexture* m_cubeMap, const ICamera* camera, Texture* depthMap, int windowWidth, int windowHeight) {

    m_shader->Bind();

    Texture::SetActiveTextureUnit(0);
    m_cubeMap->Bind();
    m_shader->SetUniform("sampler", 0);

    Texture::SetActiveTextureUnit(1);
    depthMap->Bind();
    m_shader->SetUniform("depthMap", 1);



    SetDepthTest(false);

    Matrix4f modelView = camera->GetViewMatrix();//  * Matrix4f::CreateTranslation(camera->GetPosition());

    // make sure that camera position is always zero.
    // this ensures that the skybox follows the player.
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;



    Matrix4f mvp = camera->GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
    m_shader->SetUniform("windowWidth", (float)windowWidth);
    m_shader->SetUniform("windowHeight", (float)windowHeight);

    m_indexBuffer->Bind();

    m_positionBuffer->EnableVertexAttribInterleavedWithBind();


    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);

    m_indexBuffer->Unbind();

    m_positionBuffer->DisableVertexAttribInterleavedWithBind();



    SetDepthTest(true);

    m_cubeMap->Unbind();
    m_shader->Unbind();
}

Skybox::~Skybox() {
    delete m_indexBuffer;
    delete m_positionBuffer;
    delete m_shader;
}
