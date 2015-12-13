#pragma once

#include "ewa/igeometry_object.hpp"

class PhysicsWorld;

class Car : public IGeometryObject{

private:

    IGeometryObject* geoObj;

    Car() {}

public:

    static Car* Load(PhysicsWorld* physicsWorld);
    virtual ~Car();

    virtual void Render(
	const Camera& camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap);

    virtual void RenderShadowMap(const Matrix4f& lightVp);

    virtual AABB GetModelSpaceAABB()const;

};
