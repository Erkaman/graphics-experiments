#pragma once

#include <btBulletDynamicsCommon.h>

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"


btVector3 toBtVec(const Vector3f& v);
Vector3f fromBtVec(const btVector3& v);


btQuaternion toBtQuat(const Vector4f& v);

btQuaternion toBtQuat(const Vector3f& v);
