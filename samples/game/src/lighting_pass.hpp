#pragma once

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

class ShaderProgram;
class Gbuffer;
class RandomTexture;
class Texture;
class ICamera;
class Vector4f;
class Matrix4f;
class DepthFBO;
class VBO;
class CubeMapTexture;
class ColorDepthFbo;
class ColorFBO;
class Texture2D;
class ViewFrustum;
class DualParaboloidMap;

struct PointLight {
    Vector3f m_position;
    Vector3f m_color;
    float m_radius;


    PointLight(
	Vector3f position, Vector3f color, float radius) {
	m_position = position;
	m_color = color;
     m_radius = radius;
    }
};

class LightingPass{

private:

    ShaderProgram* m_directionalShader;
    Vector2f m_screenSize;

    unsigned int m_sphereNumTriangles;
    VBO* m_sphereVertexBuffer;
    VBO* m_sphereIndexBuffer;
    ShaderProgram* m_pointShader;

    Texture2D* m_lightGridTexture;
    Vector3f* m_lightGridTextureBuffer;

    Texture2D* m_lightIndexTexture;
    float* m_lightIndexTextureBuffer;

    int m_lightIndexTextureSize;

    Vector3f* m_pointLightPosition;
    float* m_pointLightRadius;
    Vector3f* m_pointLightColor;

    void SetupShader(
	ShaderProgram* shader, Gbuffer* gbuffer, const ICamera* camera,
	const DualParaboloidMap& dualParaboloidMap,
	ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap);
    void UnsetupShader(
	ShaderProgram* shader, Gbuffer* gbuffer,
	const DualParaboloidMap& dualParaboloidMap,
	ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap);

    void DrawPointLight(const ICamera* camera, const Vector3f& position, const Vector3f& color, float radius);

    std::vector<PointLight> GetTestLights();
    std::vector<PointLight> GetTorches(const ICamera* camera, const std::vector<Vector3f>& torches);

    void DrawLights(const ICamera* camera, const std::vector<PointLight>& lights, const ViewFrustum& cameraFrustum);

public:

    LightingPass(int framebufferWidth, int framebufferHeight);

    void Render(
	Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition,
	const Matrix4f& lightVp, const DepthFBO& shadowMap, const std::vector<Vector3f>& torches,
	const DualParaboloidMap& dualParaboloidMap , ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap,
	 const ViewFrustum& cameraFrustum);

    void UpdateTextures(int lightCount);

};
