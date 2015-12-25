#include "physics_world.hpp"

#include <btBulletDynamicsCommon.h>

#include "common.hpp"

PhysicsWorld::PhysicsWorld() {

    // TODO: replace with sweep and prune broad phase instead?
    m_broadphase = new btDbvtBroadphase();

    // TODO: fine tune the physics config?
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    m_solver = new btSequentialImpulseConstraintSolver();

    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver,
									 m_collisionConfiguration);

    m_world->setGravity(btVector3(0, -10, 0));
}

PhysicsWorld::~PhysicsWorld() {
    MY_DELETE(m_world);
    MY_DELETE(m_solver);
    MY_DELETE(m_dispatcher);
    MY_DELETE(m_collisionConfiguration);
    MY_DELETE(m_broadphase);

}

void PhysicsWorld::AddRigidBody(btRigidBody* rigidBody) {
    LOG_I("AddRigidBody");
    m_world->addRigidBody(rigidBody);
    LOG_I("success AddRigidBody");

}

void PhysicsWorld::Update(const float delta) {
//    LOG_I("delta: %f", delta);
    m_world->stepSimulation(delta, 10);
}

void PhysicsWorld::AddVehicle(btActionInterface* vehicle) {
    LOG_I("AddVechicle");
    m_world->addVehicle(vehicle);
}

btVehicleRaycaster* PhysicsWorld::NewVehicleRaycaster() {
    return new btDefaultVehicleRaycaster(m_world);
}
