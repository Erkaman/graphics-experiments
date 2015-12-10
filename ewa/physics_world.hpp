#pragma once

class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btRigidBody;

class PhysicsWorld {

private:

    btDiscreteDynamicsWorld* m_world;

    btBroadphaseInterface* m_broadphase;

    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;

    btSequentialImpulseConstraintSolver* m_solver;

public:

    PhysicsWorld();
    ~PhysicsWorld();

    void AddRigidBody(btRigidBody* rigidBody);

    void Update(const float delta);

};
