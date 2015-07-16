#pragma once

#include "math/matrix4f.hpp"

class Vector3f;

class GeometryObject {

private:

    Matrix4f translationMatrix;
    Matrix4f scaleMatrix;

public:

    GeometryObject(const Vector3f& translation, const Vector3f& scale);

    Matrix4f GetModelMatrix()const;

};
