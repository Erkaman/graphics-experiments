#include "physics_world.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

#include "common.hpp"
#include "aabb.hpp"
#include "bt_util.hpp"

extern ContactAddedCallback      gContactAddedCallback;
//gContactAddedCallback = customCallback;


static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
    btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1);
	return true;
}

PhysicsWorld::PhysicsWorld(const AABB& worldBox) {


    // TODO: replace with sweep and prune broad phase instead?
    m_broadphase = new btAxisSweep3(toBtVec(worldBox.min), toBtVec(worldBox.max));

    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

    m_solver = new btSequentialImpulseConstraintSolver();

    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver,
									 m_collisionConfiguration);

    m_world->setGravity(btVector3(0, -10.0 * WORLD_SCALE, 0));

    gContactAddedCallback = CustomMaterialCombinerCallback;
    // gContactAddedCallback = contact_added_callback;

}

PhysicsWorld::~PhysicsWorld() {
    MY_DELETE(m_world);
    MY_DELETE(m_solver);
    MY_DELETE(m_dispatcher);
    MY_DELETE(m_collisionConfiguration);
    MY_DELETE(m_broadphase);
}

void PhysicsWorld::AddRigidBody(btRigidBody* rigidBody, short group, short mask) {


    m_world->addRigidBody(rigidBody);
}

void PhysicsWorld::Update(const float delta) {
//    LOG_I("delta: %f", delta);
    m_world->stepSimulation(delta, 1);
}

void PhysicsWorld::AddVehicle(btActionInterface* vehicle) {
    m_world->addVehicle(vehicle);
}

btVehicleRaycaster* PhysicsWorld::NewVehicleRaycaster() {
    return new btDefaultVehicleRaycaster(m_world);
}
