#include "geometry_object.hpp"

#include "math/vector3f.hpp"

GeometryObject::GeometryObject(const Vector3f& translation, const Vector3f& scale): translationMatrix(Matrix4f::CreateTranslation(translation)), scaleMatrix(Matrix4f::CreateScale(scale)) {
}


Matrix4f GeometryObject::GetModelMatrix()const {
    return translationMatrix * scaleMatrix;
}
