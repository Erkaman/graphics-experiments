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
class ViewFrustum;

class IGeometryObject {

public:


    // Desctructor.
    virtual ~IGeometryObject() {}

    virtual IGeometryObject* Duplicate(unsigned int id)=0;

    virtual void Update(const ViewFrustum* cameraFrustum, const ViewFrustum* lightFrustum,
	ViewFrustum** envLightFrustums)=0;

/*
    virtual void Render(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) = 0;
*/


    /*
    virtual void RenderId(const ICamera* camera)=0;
*/

    /*
    virtual void ApplyCentralForce(const Vector3f& force)=0;
    virtual void ApplyForce(const Vector3f& force, const Vector3f& relPos)=0;
    */

    virtual void AddToPhysicsWorld(PhysicsWorld* physicsWorld)=0;



    /*
      Getters and setters.
     */

    virtual AABB GetModelSpaceAABB()const=0;

    virtual btRigidBody* GetRigidBody() const=0;

    virtual btMotionState* GetMotionState() const=0;

    virtual Vector3f GetPosition() const=0;
    virtual void SetPosition(const Vector3f& position)=0;
    virtual void SetEditPosition(const Vector3f& editPosition)=0;

    virtual btQuaternion GetRotation() const=0;
    virtual void SetRotation(const btQuaternion& rotation)=0;
    virtual void SetEditRotation(const btQuaternion& editRotation)=0;

    virtual std::string GetFilename() const=0;

    virtual bool IsSelected()const=0;
    virtual void SetSelected(bool selected)=0;

    virtual unsigned int GetId()=0;


    virtual float GetScale() const=0;
    virtual void SetScale(const float scale)=0;
    virtual void SetEditScale(const float editScale)=0;

};
