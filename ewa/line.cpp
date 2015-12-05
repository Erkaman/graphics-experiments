#include "line.hpp"

#include "ewa/common.hpp"
#include "ewa/gl/shader_program.hpp"

using std::vector;

Line::Line(const Vector3f& start, const Vector3f& end, const Vector3f& color) {

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3} // pos.
	);

    m_shader = new ShaderProgram("shader_lib/primitive");

    SetStartEnd(start,end);
    SetColor(color);

}

Line::~Line() {
    MY_DELETE(m_vertexBuffer);
}


void Line::SetStart(const Vector3f& start) {
    SetStartEnd(start, m_end);
}

void Line::SetEnd(const Vector3f& end) {
    SetStartEnd(m_start, end);
}


void Line::SetColor(const Vector3f& color) {
    m_color = color;
}



void Line::SetStartEnd(const Vector3f& start, const Vector3f& end) {
    m_start = start;
    m_end = end;

    FloatVector vertices;

    m_start.Add(vertices);
    m_end.Add(vertices);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();
}

void Line::Render(const Matrix4f& vp) {

    m_shader->Bind();

    m_shader->SetUniform("mvp", vp);
    m_shader->SetUniform("color", m_color);

    m_vertexBuffer->EnableVertexAttribInterleaved();

    m_vertexBuffer->DrawVertices(GL_LINES, 2);

    m_vertexBuffer->DisableVertexAttribInterleaved();

    m_shader->Unbind();
}
