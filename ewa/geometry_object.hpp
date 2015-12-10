#pragma once

#include "math/matrix4f.hpp"

#include "geometry_object_data.hpp"

#include "gl/depth_fbo.hpp"


class Vector3f;
class VBO;
class ShaderProgram;
class Camera;
class Texture;
class Cube;

class GeometryObject {
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

    std::vector<Chunk*> m_chunks;

    ShaderProgram* m_defaultShader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.

    bool m_hasNormalMap;
    bool m_hasSpecularMap;
    bool m_hasHeightMap;

    AABB m_aabb;
    Cube* m_aabbWireframe;

    GeometryObject() {}

    void  SetModelMatrix(const Matrix4f& modelMatrix);

    Matrix4f m_modelMatrix;

    Vector3f m_position;

protected:

    void RenderVertices(ShaderProgram& shader);

public:

    static GeometryObject* Load(const std::string& filename, const Vector3f& position);

    virtual ~GeometryObject();

    // if a custom shader is necessary, set useCustomShader=true, and override this method to bind to the shader.
    virtual void Render(const Camera& camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

    virtual void RenderShadowMap(const Matrix4f& lightVp); // vp = view projection matrix.

    AABB GetModelSpaceAABB()const;

    void SetPosition(const Vector3f& position);

    void CreateCollisionShape(const CollisionShape* colShape, const EntityInfo* entityInfo);
};
