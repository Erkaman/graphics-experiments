#pragma once


class ShaderProgram;
class Camera;
class Vector4f;
class Vector3f;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"
#include "ewa/math/matrix4f.hpp"

class ShaderProgram;
class VBO;
class PerlinSeed;

class Cube : public GeometryObject{

private:

public:

    Cube(const Vector3f& position);
    ~Cube();

};
