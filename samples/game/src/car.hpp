#pragma once

class GeometryObject;

class Car {

private:

    GeometryObject* geoObj;

public:

    static Car* Load(PhysicsWorld* physicsWorld);
    virtual ~Car();

    // if a custom shader is necessary, set useCustomShader=true, and override this method to bind to the shader.
    virtual void Render(const Camera& camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

    virtual void RenderShadowMap(const Matrix4f& lightVp); // vp = view projection matrix.

    AABB GetModelSpaceAABB()const;

    void SetPosition(const Vector3f& position);

    void CreateCollisionShape(const CollisionShape* colShape, const EntityInfo* entityInfo, PhysicsWorld* physicsWorld);


};
