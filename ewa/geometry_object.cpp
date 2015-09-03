#include "geometry_object.hpp"

#include "math/vector3f.hpp"

#include "ewa/gl/vbo.hpp"

#include "ewa/common.hpp"



/*
GeometryObject::GeometryObject(const Vector3f& translation, const Vector3f& scale): translationMatrix(Matrix4f::CreateTranslation(translation)), scaleMatrix(Matrix4f::CreateScale(scale)) {
}


Matrix4f GeometryObject::GetModelMatrix()const {
    return translationMatrix * scaleMatrix;
}
*/


void GeometryObject::Init(GeometryObjectData& data) {

    m_vertexBuffer = VBO::CreateInterleaved(
	data.m_vertexAttribsSizes);

    m_indexBuffer = VBO::CreateIndex(data.m_indexType);

    m_numTriangles = data.m_numTriangles;

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(data.m_vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(data.m_indices);
    m_indexBuffer->Unbind();


}


GeometryObject::~GeometryObject() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
}

void GeometryObject::Render() {
    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

}
