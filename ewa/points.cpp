#include "points.hpp"

#include "ewa/common.hpp"
#include "ewa/gl/shader_program.hpp"

using std::vector;

Points* Points::Load(std::vector<Vector3f> points, const GLfloat pointSize, const Vector3f& color) {

    ShaderProgram* shader = ShaderProgram::Load("shader_lib/primitive");

    if(!shader) {
	return NULL;
    }

    return new Points(points, pointSize, color, shader);
}

Points::Points(std::vector<Vector3f> points, const GLfloat pointSize, const Vector3f& color, ShaderProgram* shader) {

    m_shader = shader;

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3} // pos.
	);


    SetPoints(points);
    m_color = color;
    m_pointSize = pointSize;

}


Points::~Points() {
    MY_DELETE(m_vertexBuffer);
}


void Points::SetPoints(std::vector<Vector3f> points) {
    m_points = points;
    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(points);
    m_vertexBuffer->Unbind();
}

void Points::Render(const Matrix4f& vp) {

    glPointSize(m_pointSize);

    m_shader->Bind();

    m_shader->SetUniform("mvp", vp);
    m_shader->SetUniform("color", m_color);

    m_vertexBuffer->EnableVertexAttribInterleaved();

    m_vertexBuffer->DrawVertices(GL_POINTS, m_points.size());

    m_vertexBuffer->DisableVertexAttribInterleaved();

    m_shader->Unbind();
}
