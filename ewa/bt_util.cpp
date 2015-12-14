#include "bt_util.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

btVector3 toBtVec(const Vector3f& v) {
    return btVector3(v.x,v.y,v.z);
}

btQuaternion toBtQuat(const Vector4f& v) {
    return btQuaternion(v.x,v.y,v.z, v.w);
}
