#pragma once

#include "aabb.hpp"
#include "log.hpp"

#include <LinearMath/btQuaternion.h>


class ICamera;
class Matrix4f;
class DepthFBO;
class btMotionState;
class btRigidBody;
class PhysicsWorld;

class IGeometryObject {

public:

    virtual void Render(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) = 0;

    virtual void RenderShadowMap(const Matrix4f& lightVp) = 0;

    virtual void RenderWithOutlines(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) {LOG_E("RenderWithOutlines not yet implemented!"); }

    virtual void RenderId(const ICamera* camera){LOG_E("RenderId not yet implemented!"); }

    virtual AABB GetModelSpaceAABB()const=0;

    virtual ~IGeometryObject() {}

#pragma warning( disable : 4100 ) // temporarily disable warning: "unreferenced formal parameter.

    virtual void ApplyCentralForce(const Vector3f& force) {LOG_E("ApplyCentralForce not yet implemented!"); }

    virtual void ApplyForce(const Vector3f& force, const Vector3f& relPos) {LOG_E("ApplyForce not yet implemented!"); }

    virtual btRigidBody* GetRigidBody() const {LOG_E("GetRigidBody not yet implemented!"); }

    virtual btMotionState* GetMotionState() const { LOG_E("GetMotionState not yet implemented!"); }

    virtual Vector3f GetPosition() const=0;
    virtual btQuaternion GetRotation() const { LOG_E("GetRotation not yet implemented!"); }

    virtual void AddToPhysicsWorld(PhysicsWorld* physicsWorld)=0;

    virtual void SetEditPosition(const Vector3f& editPosition) {LOG_E("SetEditPosition not yet implemented!");}

    virtual void SetPosition(const Vector3f& position) {LOG_E("SetPosition not yet implemented!");}

    virtual void SetRotation(const btQuaternion& rotation) {LOG_E("SetRotation not yet implemented!"); }

    virtual void SetEditRotation(const btQuaternion& editRotation) { LOG_E("SetEditRotation not yet implemented!"); }


    virtual std::string GetFilename() const { LOG_E("GetFilename not yet implemented!"); }

#pragma warning( default : 4100 )

};
