#pragma once

class ShaderProgram;

class SsaoPass{

private:

    ShaderProgram* m_shader;

public:

    SsaoPass();

    void Render();

};
