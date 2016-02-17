#include "lighting_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/config.hpp"

#include "gbuffer.hpp"
#include "ewa/gl/depth_fbo.hpp"
#include "ewa/gl/texture2d.hpp"


#include "ewa/camera.hpp"


LightingPass::LightingPass(int framebufferWidth, int framebufferHeight) {
    m_screenSize = Vector2f(framebufferWidth, framebufferHeight);
    m_directionalShader = ShaderProgram::Load("shader/directional");

}

void LightingPass::Render(Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {
    m_directionalShader->Bind();


    m_directionalShader->SetUniform("colorTexture", COLOR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(COLOR_TEXTURE_UNIT);
    gbuffer->GetColorTexture()->Bind();

    m_directionalShader->SetUniform("depthTexture", DEPTH_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(DEPTH_TEXTURE_UNIT);
    gbuffer->GetDepthTexture()->Bind();

    m_directionalShader->SetUniform("normalTexture", NORMAL_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(NORMAL_TEXTURE_UNIT);
    gbuffer->GetNormalTexture()->Bind();

    m_directionalShader->SetUniform("specularTexture", SPECULAR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(SPECULAR_TEXTURE_UNIT);
    gbuffer->GetSpecularTexture()->Bind();


    m_directionalShader->SetUniform("shadowMap", 6);
    Texture::SetActiveTextureUnit(6);
    shadowMap.GetRenderTargetTexture().Bind();


    m_directionalShader->SetUniform("screenSize", m_screenSize);
    m_directionalShader->SetLightUniforms(camera, lightPosition, lightVp);

    Config& config = Config::GetInstance();

    m_directionalShader->SetUniform("ambientLight", config.GetAmbientLight() );
    m_directionalShader->SetUniform("sceneLight", config.GetSceneLight() );

    Matrix4f invProj = camera->GetProjectionMatrix();
    invProj = invProj.Inverse();

    //  LOG_I("onv: %s", std::string(invProj).c_str() );

    m_directionalShader->SetUniform("invProj", invProj);
    m_directionalShader->SetUniform("proj", camera->GetProjectionMatrix());



/*
		ssao.setSampleRad(80.0f); // 40
		ssao.setBias( 0.8f); // 0.4
		ssao.setIntensity( 50.0f); // 50
		ssao.setScale( 0f);
*/



    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    gbuffer->GetColorTexture()->Unbind();
    gbuffer->GetDepthTexture()->Unbind();
    gbuffer->GetSpecularTexture()->Unbind();
    shadowMap.GetRenderTargetTexture().Unbind();


    m_directionalShader->Unbind();
}
