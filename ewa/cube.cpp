#include "cube.hpp"

#include "ewa/common.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/matrix4f.hpp"

using std::vector;


void Cube::AddQuad(
    FloatVector& vertices,
    const Vector3f& v1,
    const Vector3f& v2,
    const Vector3f& v3,
    const Vector3f& v4
    ) {
    v1.Add(vertices);
    v2.Add(vertices);

    v2.Add(vertices);
    v3.Add(vertices);

    v3.Add(vertices);
    v4.Add(vertices);

    v4.Add(vertices);
    v1.Add(vertices);

    m_numVertices += 8;
}

Cube::Cube() {

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3} // pos.
	);

    m_shader = new ShaderProgram("shader_lib/primitive");

    FloatVector vertices;
    m_numVertices = 0;

    // top side
    AddQuad(
	vertices,
	Vector3f(0,0,0),
	Vector3f(0,0,1),
	Vector3f(1,0,1),
	Vector3f(1,0,0)
	);

    // front
    AddQuad(
	vertices,
	Vector3f(0,0,0),
	Vector3f(1,0,0),
	Vector3f(1,-1,0),
	Vector3f(0,-1,0)
	);

    // back
    AddQuad(
	vertices,
	Vector3f(0,0,1),
	Vector3f(1,0,1),
	Vector3f(1,-1,1),
	Vector3f(0,-1,1)
	);

    // left side
    AddQuad(
	vertices,
	Vector3f(0,0,0),
	Vector3f(0,0,1),
	Vector3f(0,-1,1),
	Vector3f(0,-1,0)
	);

    // right side
    AddQuad(
	vertices,
	Vector3f(1,0,0),
	Vector3f(1,0,1),
	Vector3f(1,-1,1),
	Vector3f(1,-1,0)
	);




    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();


}

Cube::~Cube() {
    MY_DELETE(m_vertexBuffer);
}

void Cube::Render(const Matrix4f& vp) {

    m_shader->Bind();

    m_shader->SetUniform("mvp", vp *
			 Matrix4f::CreateTranslation(-2,0,0) *
			 Matrix4f::CreateScale(4,2,1)
	);
    m_shader->SetUniform("color", Vector3f(1.0,0.0,0.0) );

    m_vertexBuffer->EnableVertexAttribInterleaved();

    m_vertexBuffer->DrawVertices(GL_LINES, m_numVertices);

    m_vertexBuffer->DisableVertexAttribInterleaved();

    m_shader->Unbind();
}
