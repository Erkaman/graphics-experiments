#pragma once

#include "ewa/math/vector2f.hpp"

class ShaderProgram;
class Gbuffer;
class RandomTexture;
class Texture;
class ICamera;
class Vector4f;
class Matrix4f;
class DepthFBO;

class LightingPass{

private:

    ShaderProgram* m_directionalShader;
    Vector2f m_screenSize;

public:

    LightingPass(int framebufferWidth, int framebufferHeight);

    void Render(Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

};
