#pragma once


class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;
class Vector3f;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"
#include "ewa/math/matrix4f.hpp"

class ShaderProgram;
class VBO;
class PerlinSeed;

class Plane : public GeometryObject{

    private:

    ShaderProgram* m_shader;

    PerlinSeed* m_perlinSeed;

    Matrix4f m_modelMatrix;

public:

    Plane(const Vector3f& position, const bool isCellular);
    ~Plane();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

};

/*
  class GeometryObject {

  VBO vertices;
  VBO indices;

  GeometryObject(Vector3f position, string filename) {}

  Init(Vector3f position, GeometryObjectData) {}

}

GeometryObjectData {

    Texture;
    numTriangles;
    vertexBuffer
    indexBuffer

    }
*/
