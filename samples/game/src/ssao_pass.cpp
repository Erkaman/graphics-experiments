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

    Texture* colorTexture = gbuffer->GetColorTexture();
    m_shader->SetUniform("colorTexture", 0);
    Texture::SetActiveTextureUnit(0);
    colorTexture->Bind();

    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    colorTexture->Unbind();


    m_shader->Unbind();
}
