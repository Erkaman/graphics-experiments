#include "lighting_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/config.hpp"

#include "gbuffer.hpp"
#include "ewa/gl/depth_fbo.hpp"
#include "ewa/gl/texture2d.hpp"


#include "ewa/camera.hpp"

constexpr int SLICES = 10;
constexpr int STACKS = 10;

using std::vector;
using std::string;

GLushort MyGenerateSphereVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    ) {

    FloatVector vertices;
    UshortVector indices;

    vector<Vector3f> vs;


    // loop through stacks.
    for (int i = 0; i <= stacks; ++i){

	float V   = (float)i / (float) stacks;
	float phi = V * PI;

	// loop through the slices.
	for (int j = 0; j <= slices; ++j){

	    float U = (float)j / (float) slices;
	    float theta = U * (PI * 2);

	    // use spherical coordinates to calculate the positions.
	    float x = cos (theta) * sin (phi);
	    float y = cos (phi);
	    float z = sin (theta) * sin (phi);

	    Vector3f pos(x,y,z);
//	    pos *= radius;
	    pos.Add(vertices);

//	    Vector3f(x,y,z).Add(vertices);
//	    vs.push_back(Vector3f(x,y,z) );


//	    LOG_I("lol: %s",  string(Vector3f(x,y,z)).c_str()  );


	}
    }
    //  exit(1);

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){

	indices.push_back (GLushort(i) );
	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i + slices));

	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i));
	indices.push_back (GLushort(i + 1));

//	LOG_I("short: %d", GLushort(i + slices + 1));

	/*
	indices.push_back (GLushort(i + slices));
	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i));

	indices.push_back (GLushort(i + 1));
	indices.push_back (GLushort(i));
	indices.push_back (GLushort(i + slices + 1));
	*/
    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    return (GLushort)indices.size();
}



LightingPass::LightingPass(int framebufferWidth, int framebufferHeight) {
    m_screenSize = Vector2f(framebufferWidth, framebufferHeight);
    m_directionalShader = ShaderProgram::Load("shader/directional");


    m_pointShader = ShaderProgram::Load("shader/point_light");

    m_sphereVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3} // pos
	);
    m_sphereIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_sphereNumTriangles = MyGenerateSphereVertices(1, SLICES, STACKS, m_sphereVertexBuffer, m_sphereIndexBuffer);
}

void LightingPass::Render(Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {

    SetupShader(m_directionalShader, gbuffer, camera);

    m_directionalShader->SetUniform("shadowMap", 6);
    Texture::SetActiveTextureUnit(6);
    shadowMap.GetRenderTargetTexture().Bind();

    m_directionalShader->SetLightUniforms(camera, lightPosition, lightVp);

    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    shadowMap.GetRenderTargetTexture().Unbind();

    UnsetupShader(m_directionalShader, gbuffer);







    SetupShader(m_pointShader, gbuffer, camera);

    const float RADIUS = 8;

    const Matrix4f modelViewMatrix =
	camera->GetViewMatrix() *

	Matrix4f::CreateTranslation( Vector3f(0,4,0) ) *
	Matrix4f::CreateScale(RADIUS,RADIUS,RADIUS);

    const Matrix4f mvp = camera->GetProjectionMatrix() * modelViewMatrix;


    m_pointShader->SetUniform("mvp",  mvp);
    m_pointShader->SetUniform("modelViewMatrix",  modelViewMatrix);
    m_pointShader->SetUniform("radius",  (float)RADIUS);


    //GL_C(glFrontFace(GL_CW));
    GL_C(glDisable(GL_DEPTH_TEST));
   GL_C(glEnable(GL_BLEND));
   GL_C(glBlendFunc(GL_ONE, GL_ONE));

//    ::SetCullFace(false);




    //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    VBO::DrawIndices(*m_sphereVertexBuffer, *m_sphereIndexBuffer, GL_TRIANGLES, (m_sphereNumTriangles)*3);


//    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

   GL_C(glDisable(GL_BLEND));


    GL_C(glEnable(GL_DEPTH_TEST));
//    ::SetCullFace(true);


    shadowMap.GetRenderTargetTexture().Unbind();

    UnsetupShader(m_pointShader, gbuffer);


}



void LightingPass::SetupShader(ShaderProgram* shader, Gbuffer* gbuffer, const ICamera* camera) {

    shader->Bind();

    shader->SetUniform("colorTexture", COLOR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(COLOR_TEXTURE_UNIT);
    gbuffer->GetColorTexture()->Bind();

    shader->SetUniform("depthTexture", DEPTH_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(DEPTH_TEXTURE_UNIT);
    gbuffer->GetDepthTexture()->Bind();

    shader->SetUniform("normalTexture", NORMAL_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(NORMAL_TEXTURE_UNIT);
    gbuffer->GetNormalTexture()->Bind();

    shader->SetUniform("specularTexture", SPECULAR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(SPECULAR_TEXTURE_UNIT);
    gbuffer->GetSpecularTexture()->Bind();


    shader->SetUniform("screenSize", m_screenSize);


    Config& config = Config::GetInstance();

    shader->SetUniform("ambientLight", config.GetAmbientLight() );
    shader->SetUniform("sceneLight", config.GetSceneLight() );

    Matrix4f invProj = camera->GetProjectionMatrix();
    invProj = invProj.Inverse();

    //  LOG_I("onv: %s", std::string(invProj).c_str() );

    shader->SetUniform("invProj", invProj);
    shader->SetUniform("proj", camera->GetProjectionMatrix());
}

void LightingPass::UnsetupShader(ShaderProgram* shader, Gbuffer* gbuffer) {
    shader->Unbind();


    gbuffer->GetColorTexture()->Unbind();
    gbuffer->GetDepthTexture()->Unbind();
    gbuffer->GetSpecularTexture()->Unbind();

}
