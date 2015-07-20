#pragma once

#include "geometry_object.hpp"

#include "math/vector3f.hpp"


class VBO;
class ShaderProgram;
class Camera;
class Vector4f;
class ValueNoiseSeed;

class Sphere : public GeometryObject{

private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    ShaderProgram* m_shader;

    ValueNoiseSeed* m_perlinSeed;

    float m_delta;

    Vector3f m_sunDirection;

    float m_azimuthAngle;
    float m_elevationAngle;

    float m_reileighCoefficient;
    float m_mieCoefficient;
    float m_mieDirectionalG;
    float m_turbidity;

public:

    Sphere(const float radius, const int slices, const int stacks);
    ~Sphere();


    void Draw(const Camera& camera);
    void Update(const float delta);

    void UpdateSunDirection();


};
