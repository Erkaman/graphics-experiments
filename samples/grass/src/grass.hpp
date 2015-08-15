#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"

class Grass{

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    ShaderProgram* m_shader;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    Texture* m_grassTexture;

    void AddQuad(FloatVector& vertices, UshortVector& indices,
	     const Vector3f& bottomLeft, const Vector3f& bottomRight,
		 const Vector3f& topLeft, const Vector3f& topRight);

    void MakeGrass();

public:

    //
    Grass();
    ~Grass();

    void Draw(const Camera& camera, const Vector4f& lightPosition);
};
