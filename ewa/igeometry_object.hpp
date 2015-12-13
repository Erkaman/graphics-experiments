#pragma once

#include "aabb.hpp"

class Camera;
class Matrix4f;
class DepthFBO;

class IGeometryObject {

public:

    virtual void Render(
	const Camera& camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) = 0;

    virtual void RenderShadowMap(const Matrix4f& lightVp) = 0;

    virtual AABB GetModelSpaceAABB()const = 0;

};
