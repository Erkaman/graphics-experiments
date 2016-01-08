#include "physics_world.hpp"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <bullet/BulletCollision/CollisionShapes/btTriangleShape.h>

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

#include "common.hpp"

extern ContactAddedCallback      gContactAddedCallback;
//gContactAddedCallback = customCallback;

void contact_added_callback_obj (btManifoldPoint& cp,
                                 const btCollisionObjectWrapper* colObjWrapper,
                                 int partId, int index)
{
    const btCollisionObject* colObj = colObjWrapper->m_collisionObject;

	LOG_I("lol run");


        (void) partId;
        (void) index;
        const btCollisionShape *shape =colObj->getCollisionShape();

        if (shape->getShapeType() != TRIANGLE_SHAPE_PROXYTYPE) return;
        const btTriangleShape *tshape =
               static_cast<const btTriangleShape*>(colObj->getCollisionShape());


        const btCollisionShape *parent =
	    colObjWrapper->m_parent->m_collisionObject->getCollisionShape();

	    //colObj->getRootCollisionShape();
        if (parent == NULL) return;
        if (parent->getShapeType() != TRIANGLE_MESH_SHAPE_PROXYTYPE) return;


        btTransform orient = colObj->getWorldTransform();
        orient.setOrigin( btVector3(0.0f,0.0f,0.0f ) );

        btVector3 v1 = tshape->m_vertices1[0];
        btVector3 v2 = tshape->m_vertices1[1];
        btVector3 v3 = tshape->m_vertices1[2];

        btVector3 normal = (v2-v1).cross(v3-v1);

        normal = orient * normal;
        normal.normalize();

        btScalar dot = normal.dot(cp.m_normalWorldOnB);
        btScalar magnitude = cp.m_normalWorldOnB.length();
        normal *= dot > 0 ? magnitude : -magnitude;

        cp.m_normalWorldOnB = normal;
}

bool contact_added_callback (btManifoldPoint& cp,
                             const btCollisionObjectWrapper * colObj0,
                             int partId0, int index0,
                             const btCollisionObjectWrapper * colObj1,
                             int partId1, int index1)
{

    contact_added_callback_obj(cp, colObj0, partId0, index0);
    contact_added_callback_obj(cp, colObj1, partId1, index1);
        //std::cout << to_ogre(cp.m_normalWorldOnB) << std::endl;
        return true;
}


static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
    btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1);
}

PhysicsWorld::PhysicsWorld() {



    // TODO: replace with sweep and prune broad phase instead?
    m_broadphase = new btDbvtBroadphase();

    // TODO: fine tune the physics config?
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

void PhysicsWorld::AddRigidBody(btRigidBody* rigidBody) {


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
