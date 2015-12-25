#include "bt_util.hpp"


btVector3 toBtVec(const Vector3f& v) {
    return btVector3(v.x,v.y,v.z);
}

Vector3f fromBtVec(const btVector3& v) {
    return Vector3f(v.x(), v.y(), v.z());
}


btQuaternion toBtQuat(const Vector4f& v) {
    return btQuaternion(v.x,v.y,v.z, v.w);
}

btQuaternion toBtQuat(const Vector3f& v) {
    btQuaternion quat;
    quat.setEuler(v.x,v.y,v.z);

    return quat;

}
