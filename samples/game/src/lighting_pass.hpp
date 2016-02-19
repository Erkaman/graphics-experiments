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
class VBO;

class LightingPass{

private:

    ShaderProgram* m_directionalShader;
    Vector2f m_screenSize;

    unsigned int m_sphereNumTriangles;
    VBO* m_sphereVertexBuffer;
    VBO* m_sphereIndexBuffer;
    ShaderProgram* m_pointShader;

    void SetupShader(ShaderProgram* shader, Gbuffer* gbuffer, const ICamera* camera);
    void UnsetupShader(ShaderProgram* shader, Gbuffer* gbuffer);


public:

    LightingPass(int framebufferWidth, int framebufferHeight);

    void Render(Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

};
