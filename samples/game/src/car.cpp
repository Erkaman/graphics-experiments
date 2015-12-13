#include "car.hpp"

#include "ewa/geometry_object.hpp"


Car* Car::Load(PhysicsWorld* physicsWorld) {
    Car* car = new Car();

    car->geoObj = GeometryObject::Load("obj/car.eob", Vector3f(0,-1.5,0), physicsWorld );


    if(!car) {
	return NULL;
    }

    return car;
}


Car::~Car() {
    delete geoObj;
}

void Car::Render(
    const Camera& camera,
    const Vector4f& lightPosition,
    const Matrix4f& lightVp,
    const DepthFBO& shadowMap) {
    geoObj->Render(camera, lightPosition, lightVp, shadowMap);
}

void Car::RenderShadowMap(const Matrix4f& lightVp) {
    geoObj->RenderShadowMap(lightVp);
}

AABB Car::GetModelSpaceAABB()const {
    return geoObj->GetModelSpaceAABB();
}


void Car::Update(const float delta) {
//    LOG_I("car update");

    Vector3f force = 600.0f * delta * Vector3f(-1,0,0);

    geoObj->ApplyForce(force);

    // (-1,0,0)

    //applyCentralForce
}
