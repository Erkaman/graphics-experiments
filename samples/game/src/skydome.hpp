#pragma once

#include "geometry_object.hpp"


class VBO;
class ShaderProgram;
class Camera;
class ValueNoiseSeed;

class Skydome : public GeometryObject{

private:

    /*
      There variables are used to draw the dome:
     */

    unsigned int m_domeNumTriangles;
    VBO* m_domeVertexBuffer;
    VBO* m_domeIndexBuffer;
    ShaderProgram* m_domeShader;

    float m_delta;

    ValueNoiseSeed* m_perlinSeed;

    /*
      These are used to draw the sun:
     */

public:

    Skydome(const float radius, const int slices, const int stacks);
    ~Skydome();


    void Draw(const Camera& camera);
    void Update(const float delta);

    void MakeSun();

};
