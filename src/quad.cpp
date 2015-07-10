#include "quad.hpp"

#include "gl/vbo.hpp"

#include "math/vector3f.hpp"
#include "math/vector2f.hpp"

using std::vector;


Quad::Quad(const Vector2f c1, const Vector2f& c2) {

    m_vertexBuffer = VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX, VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{2, 2}
						    );

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    FloatVector vertices;
    UshortVector indices;

    const float SCALE = 1.0f;


    c1.Add(vertices);
    Vector2f(0.0f,0.0f).Add(vertices);

    Vector2f(c2.x,c1.y).Add(vertices);
    Vector2f(1.0f,0.0f).Add(vertices);

    c2.Add(vertices);
    Vector2f(1.0f,1.0f).Add(vertices);


    Vector2f(c1.x,c2.y).Add(vertices);
    Vector2f(0.0f,1.0f).Add(vertices);


    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    m_numTriangles = 2;

}

Quad::~Quad() {
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

void Quad::Draw() {
    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);
}
