#pragma once

#include "math/matrix4f.hpp"

#include "geometry_object_data.hpp"

#include "igeometry_object.hpp"


class Vector3f;
class VBO;
class ShaderProgram;
class Texture;
class Cube;
class PhysicsWorld;
class btRigidBody;
class btMotionState;

class GeometryObject : public IGeometryObject {
private:

    struct Chunk {
	VBO* m_vertexBuffer;
	VBO* m_indexBuffer;
	GLuint m_numTriangles;

	// the material.
	Texture* m_texture;
	Texture* m_normalMap;
	Texture* m_specularMap;

	float m_shininess;
	Vector3f m_specularColor;

    };

    GeometryObjectData* m_data;


    std::vector<Chunk*> m_chunks;

    ShaderProgram* m_defaultShader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.

    bool m_hasNormalMap;
    bool m_hasSpecularMap;
    bool m_hasHeightMap;

    AABB m_aabb;
    Cube* m_aabbWireframe;

    btRigidBody* m_rigidBody;
    btMotionState* m_motionState;

    GeometryObject();

    Vector3f m_position;
    Matrix4f m_rotation;

    Vector3f m_editPosition;

    void CreateCollisionShape(const CollisionShape* colShape, const EntityInfo* entityInfo, PhysicsWorld* physicsWorld);

    Matrix4f GetModelMatrix()const;

protected:

    void RenderVertices(ShaderProgram& shader);

public:

    static GeometryObject* Load(const std::string& filename, const Vector3f& position,PhysicsWorld* physicsWorld);

    virtual ~GeometryObject();

    virtual void Render(

	const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

    virtual void RenderShadowMap(const Matrix4f& lightVp); // vp = view projection matrix.

    AABB GetModelSpaceAABB()const;

    void SetPosition(const Vector3f& position);
    void SetRotation(const Matrix4f& rotation);
    virtual void SetEditPosition(const Vector3f& editPosition);




    virtual void ApplyCentralForce(const Vector3f& force);

    virtual void ApplyForce(const Vector3f& force, const Vector3f& relPos);

    virtual btRigidBody* GetRigidBody() const;

    virtual btMotionState* GetMotionState() const;

    virtual Vector3f GetPosition() const;

    virtual void AddToPhysicsWorld(PhysicsWorld* physicsWorld);

};
