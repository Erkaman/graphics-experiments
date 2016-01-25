#include "ssao_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "gbuffer.hpp"

SsaoPass::SsaoPass() {
    m_shader = ShaderProgram::Load("shader/ssao");
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


    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    gbuffer->GetColorTexture()->Unbind();
    gbuffer->GetDepthTexture()->Unbind();
    gbuffer->GetPositionTexture()->Unbind();

    m_shader->Unbind();
}
