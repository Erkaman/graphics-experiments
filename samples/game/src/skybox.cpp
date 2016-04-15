#include "skybox.hpp"
#include "dual_paraboloid_map.hpp"

#include "ewa/camera.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector4f.hpp"

#include "ewa/resource_manager.hpp"


using std::vector;
using std::string;

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

    vector<string> defaultDefines;


    {
	vector<string> defines(defaultDefines);

	string shaderName = "shader/skybox_deferred";

	m_deferredShader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }


    {
	vector<string> defines(defaultDefines);

	string shaderName = "shader/skybox_forward";

	m_forwardShader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }

    {
	vector<string> defines(defaultDefines);
	defines.push_back("PARABOLOID");

	string shaderName = "shader/skybox_forward";

	m_envMapShader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }


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

void Skybox::DrawDeferred(CubeMapTexture* m_cubeMap, const ICamera* camera, Texture* depthMap, int windowWidth, int windowHeight) {

    m_deferredShader->Bind();

    Texture::SetActiveTextureUnit(0);
    m_cubeMap->Bind();
    m_deferredShader->SetUniform("sampler", 0);

    Texture::SetActiveTextureUnit(1);
    depthMap->Bind();
    m_deferredShader->SetUniform("depthMap", 1);



    SetDepthTest(false);

    Matrix4f modelView = camera->GetViewMatrix();

    // make sure that camera position is always zero.
    // this ensures that the skybox follows the player.
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    m_deferredShader->SetShaderUniforms(
	Matrix4f::CreateIdentity(),
	modelView,
	camera->GetProjectionMatrix());


    m_deferredShader->SetUniform("windowWidth", (float)windowWidth);
    m_deferredShader->SetUniform("windowHeight", (float)windowHeight);

    m_indexBuffer->Bind();

    m_positionBuffer->EnableVertexAttribInterleavedWithBind();


    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);

    m_indexBuffer->Unbind();

    m_positionBuffer->DisableVertexAttribInterleavedWithBind();

    SetDepthTest(true);

    m_cubeMap->Unbind();
    m_deferredShader->Unbind();
}

void Skybox::DrawForward(CubeMapTexture* m_cubeMap, const ICamera* camera) {

    m_forwardShader->Bind();

    Texture::SetActiveTextureUnit(0);
    m_cubeMap->Bind();
    m_forwardShader->SetUniform("sampler", 0);



    SetDepthTest(false);

    Matrix4f modelView = camera->GetViewMatrix();//  * Matrix4f::CreateTranslation(camera->GetPosition());

    // make sure that camera position is always zero.
    // this ensures that the skybox follows the player.
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    m_forwardShader->SetShaderUniforms(
	Matrix4f::CreateIdentity(),
	modelView,
	camera->GetProjectionMatrix());


    m_indexBuffer->Bind();

    m_positionBuffer->EnableVertexAttribInterleavedWithBind();


    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);

    m_indexBuffer->Unbind();

    m_positionBuffer->DisableVertexAttribInterleavedWithBind();

    SetDepthTest(true);

    m_cubeMap->Unbind();
    m_forwardShader->Unbind();
}

void Skybox::DrawEnvMap(CubeMapTexture* m_cubeMap, Paraboloid& paraboloid) {

    m_envMapShader->Bind();

    Texture::SetActiveTextureUnit(0);
    m_cubeMap->Bind();
    m_envMapShader->SetUniform("sampler", 0);



    SetDepthTest(false);

    Matrix4f view = paraboloid.m_viewMatrix;

    // make sure that camera position is always zero.
    // this ensures that the skybox follows the player.
    view.m03 = 0;
    view.m13 = 0;
    view.m23 = 0;

    paraboloid.SetParaboloidUniforms(
		*m_envMapShader,
		Matrix4f::CreateIdentity(),
		view,
		Matrix4f::CreateIdentity(),

		paraboloid.m_position,
		Vector4f(0,0,0,0) );

    /*
    m_envMapShader->SetShaderUniforms(
	Matrix4f::CreateIdentity(),
	modelView,
	camera->GetProjectionMatrix());
    */

    m_indexBuffer->Bind();

    m_positionBuffer->EnableVertexAttribInterleavedWithBind();

    GL_C(glEnable(GL_CLIP_DISTANCE0));

    m_indexBuffer->DrawIndices(GL_TRIANGLES, m_numIndices);

    GL_C(glDisable(GL_CLIP_DISTANCE0));


    m_indexBuffer->Unbind();

    m_positionBuffer->DisableVertexAttribInterleavedWithBind();

    SetDepthTest(true);

    m_cubeMap->Unbind();
    m_envMapShader->Unbind();

}


Skybox::~Skybox() {
    delete m_indexBuffer;
    delete m_positionBuffer;
    delete m_deferredShader;
}
