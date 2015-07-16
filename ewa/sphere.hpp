#pragma once

#include "geometry_object.hpp"

class VBO;
class ShaderProgram;
class Camera;
class Vector4f;
class PerlinSeed;

class Sphere : public GeometryObject{

private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    ShaderProgram* m_shader;

    PerlinSeed* m_perlinSeed;

    float m_delta;

public:

    Sphere(const float radius, const int slices, const int stacks);
    ~Sphere();


    void Draw(const Camera& camera);
    void Update(const float delta);


};
