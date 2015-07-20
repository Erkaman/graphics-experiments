#pragma once

#include "geometry_object.hpp"


class VBO;
class ShaderProgram;
class Camera;
class ValueNoiseSeed;

class Skydome : public GeometryObject{

private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    ShaderProgram* m_shader;

    ValueNoiseSeed* m_perlinSeed;

    float m_delta;

public:

    Skydome(const float radius, const int slices, const int stacks);
    ~Skydome();


    void Draw(const Camera& camera);
    void Update(const float delta);

};
