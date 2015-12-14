#pragma once

#include <btBulletDynamicsCommon.h>

class Vector3f;
class Vector4f;

btVector3 toBtVec(const Vector3f& v);

btQuaternion toBtQuat(const Vector4f& v);
