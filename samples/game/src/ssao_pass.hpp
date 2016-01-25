#pragma once

#include "ewa/math/vector2f.hpp"

class ShaderProgram;
class Gbuffer;
class RandomTexture;
class Texture;

class SsaoPass{

private:

    ShaderProgram* m_shader;
    Texture* m_randomTexture;
    Vector2f m_screenSize;

public:

    SsaoPass(int framebufferWidth, int framebufferHeight);

    void Render(Gbuffer* gbuffer);

};
