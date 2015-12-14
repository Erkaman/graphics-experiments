#pragma once

#include "aabb.hpp"
#include "log.hpp"

class Camera;
class Matrix4f;
class DepthFBO;
class btRigidBody;

class IGeometryObject {

public:

    virtual void Render(
	const Camera& camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) = 0;

    virtual void RenderShadowMap(const Matrix4f& lightVp) = 0;

    virtual AABB GetModelSpaceAABB()const=0;

    virtual ~IGeometryObject() {}

    virtual void ApplyCentralForce(const Vector3f& force) {LOG_E("ApplyCentralForce not yet implemented!"); }

    virtual void ApplyForce(const Vector3f& force, const Vector3f& relPos) {LOG_E("ApplyForce not yet implemented!"); }

    virtual btRigidBody* GetRigidBody() const {LOG_E("GetRigidBody not yet implemented!"); }

};
