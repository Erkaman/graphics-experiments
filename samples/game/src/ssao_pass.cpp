#include "ssao_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "gbuffer.hpp"

#include "ewa/gl/texture2d.hpp"


SsaoPass::SsaoPass(int framebufferWidth, int framebufferHeight) {
    m_screenSize = Vector2f(framebufferWidth, framebufferHeight);
    m_shader = ShaderProgram::Load("shader/ssao");

    m_randomTexture = Texture2D::Load("img/random_tex.png");

    m_randomTexture->Bind();
    m_randomTexture->SetTextureRepeat();
    m_randomTexture->SetMagMinFilters(GL_NEAREST);
    m_randomTexture->Unbind();
}

void SsaoPass::Render(Gbuffer* gbuffer) {
    m_shader->Bind();


    m_shader->SetUniform("colorTexture", COLOR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(COLOR_TEXTURE_UNIT);
    gbuffer->GetColorTexture()->Bind();

    m_shader->SetUniform("depthTexture", DEPTH_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(DEPTH_TEXTURE_UNIT);
    gbuffer->GetDepthTexture()->Bind();

    m_shader->SetUniform("normalTexture", NORMAL_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(NORMAL_TEXTURE_UNIT);
    gbuffer->GetNormalTexture()->Bind();

    m_shader->SetUniform("positionTexture", POSITION_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(POSITION_TEXTURE_UNIT);
    gbuffer->GetPositionTexture()->Bind();

    m_shader->SetUniform("randomTexture", POSITION_TEXTURE_UNIT+1);
    Texture::SetActiveTextureUnit(POSITION_TEXTURE_UNIT+1);
    m_randomTexture->Bind();


    m_shader->SetUniform("screenSize", m_screenSize);

    m_shader->SetUniform("intensity", 50.0f);
    m_shader->SetUniform("bias", 0.8f);
    m_shader->SetUniform("scale", 0.1f);
    m_shader->SetUniform("sampleRad", 3.0f);


/*
		ssao.setSampleRad(80.0f); // 40
		ssao.setBias( 0.8f); // 0.4
		ssao.setIntensity( 50.0f); // 50
		ssao.setScale( 0f);
*/



    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    gbuffer->GetColorTexture()->Unbind();
    gbuffer->GetDepthTexture()->Unbind();
    gbuffer->GetPositionTexture()->Unbind();
    m_randomTexture->Unbind();


    m_shader->Unbind();
}
