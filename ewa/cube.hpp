#pragma once

#include "ewa/gl/vbo.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/matrix4f.hpp"

class ShaderProgram;
class Matrix4f;

// class for drawing wireframe cubes.
class Cube {

private:

    VBO* m_vertexBuffer;

    ShaderProgram* m_shader;
    Matrix4f m_modelMatrix;

    void AddQuad(
	FloatVector& vertices,
	const Vector3f& v1,
	const Vector3f& v2,
	const Vector3f& v3,
	const Vector3f& v4
	);

    GLushort m_numVertices;

    Cube(ShaderProgram* shader);


public:



    static Cube* Load();

    ~Cube();

    void Render(const Matrix4f& vp);

    void SetModelMatrix(const Matrix4f& modelMatrix) ;

};
