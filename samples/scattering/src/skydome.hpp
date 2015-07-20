#pragma once

#include "geometry_object.hpp"

#include "math/vector3f.hpp"

class VBO;
class ShaderProgram;
class Camera;
class Vector4f;

class Skydome : public GeometryObject{

private:

    unsigned int m_numTriangles;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    ShaderProgram* m_shader;

    float m_delta;

    Vector3f m_sunDirection;

    float m_azimuthAngle;
    float m_elevationAngle;

    float m_reileighCoefficient;
    float m_mieCoefficient;
    float m_mieDirectionalG;
    float m_turbidity;

public:

    Skydome(const float radius, const int slices, const int stacks);
    ~Skydome();


    void Draw(const Camera& camera);
    void Update(const float delta);

    void UpdateSunDirection();


};
