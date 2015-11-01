#pragma once

#include "math/matrix4f.hpp"

#include "geometry_object_data.hpp"

class Vector3f;
class VBO;
class ShaderProgram;
class Camera;
class Texture;

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

    bool m_hasNormalMap;
    bool m_hasSpecularMap;

protected:

    Matrix4f m_modelMatrix;

    void RenderVertices(ShaderProgram& shader);

public:

    void Init(GeometryObjectData&  data, const bool useCustomShader = false);
    void Init(const std::string& filename, const bool useCustomShader = false);

    GeometryObject() {}
    virtual ~GeometryObject();

    // if a custom shader is necessary, set useCustomShader=true, and override this method to bind to the shader.
    virtual void Render(const Camera& camera, const Vector4f& lightPosition);

    void  SetModelMatrix(Matrix4f modelMatrix);
};
