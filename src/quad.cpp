#include "quad.hpp"


using std::vector;

Quad::Quad(){

    m_vertexBuffer = std::unique_ptr<VBO>(VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{2,2}
						    ));

    m_indexBuffer = std::unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_SHORT));

    FloatVector vertices;
    UshortVector indices;

    const float SCALE = 1.0f;


    Vector3f(0,0).Add(vertices);
    Vector2f(0,0).Add(vertices);

    Vector3f(SCALE,0).Add(vertices);
    Vector2f(SCALE,0).Add(vertices);

    Vector3f(SCALE,SCALE).Add(vertices);
    Vector2f(SCALE,SCALE).Add(vertices);

    Vector3f(0,SCALE).Add(vertices);
    Vector2f(0,SCALE).Add(vertices);

    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(0);

    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    m_numTriangles = 2;

}

void Quad::Draw() {
    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);
}
