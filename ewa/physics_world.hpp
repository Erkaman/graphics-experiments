#pragma once

class btDynamicsWorld;

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btRigidBody;
class btActionInterface;

struct btVehicleRaycaster;

class AABB;

class PhysicsWorld {

private:

    btDynamicsWorld* m_world;

    btBroadphaseInterface* m_broadphase;

    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;

    btSequentialImpulseConstraintSolver* m_solver;

public:

    PhysicsWorld(const AABB& worldBox);
    ~PhysicsWorld();

    void AddRigidBody(btRigidBody* rigidBody, short group, short mask);
    void AddVehicle(btActionInterface* vehicle);

    btVehicleRaycaster* NewVehicleRaycaster();

    void Update(const float delta);
};
