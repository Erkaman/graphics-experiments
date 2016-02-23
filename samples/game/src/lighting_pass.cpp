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

void LightingPass::Render(Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap, const std::vector<Vector3f>& torches) {

    SetupShader(m_directionalShader, gbuffer, camera);

    m_directionalShader->SetUniform("shadowMap", 6);
    Texture::SetActiveTextureUnit(6);
    shadowMap.GetRenderTargetTexture().Bind();

    m_directionalShader->SetLightUniforms(camera, lightPosition, lightVp);

    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    shadowMap.GetRenderTargetTexture().Unbind();

    UnsetupShader(m_directionalShader, gbuffer);

    shadowMap.GetRenderTargetTexture().Unbind();




    SetupShader(m_pointShader, gbuffer, camera);



    GL_C(glFrontFace(GL_CW));
    GL_C(glDisable(GL_DEPTH_TEST));

    GL_C(glEnable(GL_BLEND));
    GL_C(glBlendFunc(GL_ONE, GL_ONE));


    //DrawTestLights(camera);

    DrawTorches(camera, torches);





    GL_C(glDisable(GL_BLEND));
    GL_C(glFrontFace(GL_CCW));
    GL_C(glEnable(GL_DEPTH_TEST));

    UnsetupShader(m_pointShader, gbuffer);
}


void LightingPass::DrawTorches(const ICamera* camera, const std::vector<Vector3f>& torches) {
    for(Vector3f torch : torches ) {

	DrawPointLight(camera, torch, Vector3f(
			   0.4,0.4,0

			   ),  20.0f);
    }
}

void LightingPass::DrawTestLights(const ICamera* camera) {
    int MIN_X = -2;
    int MAX_X = +2;

    int MIN_Z = -2;
    int MAX_Z = +2;

    for(int x = MIN_X; x <= MAX_X; ++x) {

	for(int z = MIN_Z; z <= MAX_Z; ++z) {


	    int mod = ((x-MIN_X)+(z-MIN_Z)) % 3;


	    float g;

	    if(mod == 0)
		g = 0;
	    else if(mod == 1)
		g = 0.5;
	    else
		g = 1;

	    DrawPointLight(camera, Vector3f(40 * x,4, 40 * z), Vector3f(
			       (float)(x-MIN_X) / (MAX_X-MIN_X),
			       g,
			       (float)(z-MIN_Z) / (MAX_Z-MIN_Z)
			       ), 			       30.0f);

	}

    }
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

void LightingPass::DrawPointLight(const ICamera* camera, const Vector3f& position, const Vector3f& color, float radius) {

    const Matrix4f modelViewMatrix =
	camera->GetViewMatrix() *

	Matrix4f::CreateTranslation( position ) *
	Matrix4f::CreateScale(radius,radius,radius);

    const Matrix4f mvp = camera->GetProjectionMatrix() * modelViewMatrix;


    m_pointShader->SetUniform("mvp",  mvp);
    m_pointShader->SetUniform("modelViewMatrix",  modelViewMatrix);
    m_pointShader->SetUniform("radius",  (float)radius);
    m_pointShader->SetUniform("color",  color  ) ;


    VBO::DrawIndices(*m_sphereVertexBuffer, *m_sphereIndexBuffer, GL_TRIANGLES, (m_sphereNumTriangles)*3);

}
