#pragma once


class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;
class Vector3f;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"

class ShaderProgram;
class VBO;
class PerlinSeed;

class Plane : public GeometryObject{

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    ShaderProgram* m_shader;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    PerlinSeed* m_perlinSeed;

public:

    //
    Plane(const Vector3f& position, const Vector3f& scale, const bool isCellular);
    ~Plane();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

};
