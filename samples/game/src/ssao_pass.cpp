#include "ssao_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

SsaoPass::SsaoPass() {
    m_shader = ShaderProgram::Load("shader/ssao");
}

void SsaoPass::Render() {
    m_shader->Bind();
    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));
    m_shader->Unbind();
}
