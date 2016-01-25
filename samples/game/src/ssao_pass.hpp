#pragma once

class ShaderProgram;
class Gbuffer;

class SsaoPass{

private:

    ShaderProgram* m_shader;

public:

    SsaoPass();

    void Render(Gbuffer* gbuffer);

};
