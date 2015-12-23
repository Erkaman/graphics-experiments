#pragma once

#include "ewa/igeometry_object.hpp"

class PhysicsWorld;
struct btVehicleRaycaster;
class btRaycastVehicle;

class Car : public IGeometryObject{

private:

    IGeometryObject* m_geoObj;

    Car(IGeometryObject* geoObj, PhysicsWorld* physicsWorld);

    btVehicleRaycaster* m_vehicleRaycaster;

    btRaycastVehicle* m_raycastVehicle;

public:

    static Car* Load(PhysicsWorld* physicsWorld, const Vector3f& position);
    virtual ~Car();

    virtual void Render(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap);

    virtual void RenderShadowMap(const Matrix4f& lightVp);

    virtual AABB GetModelSpaceAABB()const;

    void Update();

    Vector3f GetForwardVector()const;

    virtual Vector3f GetPosition() const;


};

//could be that the heightmap normal calulcaiton does not handle steep variations in terrain, like game gems said.
