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
    int i
    ) {

    /*
    float ax=0.5f, ay=0.5f, az=0.5f;
    float bx=0.5f, by=-0.5f, bz=0.5f;
    float cx=-0.5f, cy=-0.5f, cz=0.5f;
    float dx=-0.5f, dy=0.5f, dz=0.5f;
    */


    //  LOG_I("YARU: %d", i );


    const GLushort base = (GLushort)(positions.size() / 3);

    int start = positions.size();

    int N = 10; // degree of tesselation. means  quads.

    float xmin = -0.5;
    float xmax = +0.5;
    float ymin = -0.5;
    float ymax = +0.5;

    for(int row = 0; row <= N; ++row) {

	float y = (row / (float)N)*(ymax-ymin) + ymin;

	for(int col = 0; col <= N; ++col) {

	    float x= (col / (float)N)*(xmax-xmin) + xmin;

	    positions.push_back(x);
	    positions.push_back(y);
	    positions.push_back(0.5f);



	}

    }




    /*

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
    */

    int end = positions.size();

    for(int j = start; j < end; j+=3) {

	float x = positions[j+0];
	float y = positions[j+1];
	float z = positions[j+2];

	if(i == 0) { // front

	} else if(i==1) { // back
	    z *= -1.0;
	    x *= -1.0;
	} else if(i==2) { // top

	    float x2 = -y;
	    float y2 = +z;
	    float z2 = -x;

	    x = x2;
	    y = y2;
	    z = z2;
	} else if(i==3) { // bottom.

	    float x2 = +y;
	    float y2 = -z;
	    float z2 = -x;

	    x = x2;
	    y = y2;
	    z = z2;
	} else if(i==4) { // right

	    float x2 = +z;
	    float y2 = +y;
	    float z2 = -x;

	    x = x2;
	    y = y2;
	    z = z2;
	} else if(i==5) { // left

	    float x2 = -z;
	    float y2 = +y;
	    float z2 = +x;

	    x = x2;
	    y = y2;
	    z = z2;
	}

	positions[j+0] = x;
	positions[j+1] = y;
	positions[j+2] = z;
    }

    for(int row = 0; row <= (N-1); ++row) {

	for(int col = 0; col <= (N-1); ++col) {

	    int i = row * (N+1) + col;

	    int i0 = i+0;
	    int i1 = i+1;
	    int i2 = i + (N+1) + 0;
	    int i3 = i + (N+1) + 1;


	    indices.push_back(base + i2);
	    indices.push_back(base + i1);
	    indices.push_back(base + i0);

	    indices.push_back(base + i1);
	    indices.push_back(base + i2);
	    indices.push_back(base + i3);
	}
    }


    /*
    indices.push_back(base);
    indices.push_back(base + 2);
    indices.push_back(base + 1);


    indices.push_back(base);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
    */

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

    // front
    AddFace(positions, indices,
	0);


    // back
    AddFace(positions, indices,
	1);

    AddFace(positions, indices,
	    2);     // top


    AddFace(positions, indices,
	    3);     // bottom

    AddFace(positions, indices,
	    4);     // right

    AddFace(positions, indices,
	    5);     // left


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
