#include "grass.hpp"

#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

using std::vector;

void Grass::AddQuad(FloatVector& vertices, UshortVector& indices,
	     const Vector3f& bottomLeft, const Vector3f& bottomRight,
	     const Vector3f& topLeft, const Vector3f& topRight) {

    const Vector3f n = Vector3f::Cross(topRight - bottomRight,bottomRight - bottomLeft).Normalize();

    // LOG_I("normal %s", tos(n).c_str());

    bottomLeft.Add(vertices);
    n.Add(vertices);

    bottomRight.Add(vertices);
    n.Add(vertices);

    topLeft.Add(vertices);
    n.Add(vertices);

    topRight.Add(vertices);
    n.Add(vertices);


    GLushort baseIndex = indices.size() / 3;

    indices.push_back(baseIndex+0);
    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+2);

    indices.push_back(baseIndex+1);
    indices.push_back(baseIndex+2);
    indices.push_back(baseIndex+3);

    m_numTriangles += 2;

}


Grass::Grass( ){

    m_shader = new ShaderProgram("shader/grass");


    m_vertexBuffer = VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_NORMAL_ATTRIB_INDEX},
						    vector<GLuint>{3,3}
			  );


    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    MakeGrass();


}

void Grass::Draw(const Camera& camera, const Vector4f& lightPosition) {


    //  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    SetCullFace(false);

    m_shader->Bind();

    m_shader->SetPhongUniforms(

	Matrix4f::CreateIdentity()
	 , camera, lightPosition);


    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_shader->Unbind();


    // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


    SetCullFace(true);

}

Grass::~Grass() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_shader);
}

void Grass::MakeGrass() {
    FloatVector vertices;
    UshortVector indices;
    m_numTriangles = 0;

    const Vector2f grassBeg(2,6); // x,y
    const Vector2f grassEnd(30,33); // x,y.

    Vector2f pos(grassBeg);
    Vector2f growingDir(0,1);
    growingDir.Normalize();

    while(true) {

	if(pos.x >= grassEnd.x || pos.y >= grassEnd.y) {
	    break;
	}

	const Vector2f partBeg = pos;

	const Vector2f partEnd = pos + growingDir * 0.05;

//	LOG_I("partbeg: %s", tos(partBeg).c_str() );
//	LOG_I("growingDir: %s", tos(growingDir).c_str() );

	AddQuad(vertices,indices,
		Vector3f(partBeg.x,partBeg.y,0),
		Vector3f(partBeg.x,partBeg.y,0.1f),
		Vector3f(partEnd.x, partEnd.y,0),
		Vector3f(partEnd.x, partEnd.y,0.1f)
	    );

/*
  AddQuad(vertices,indices,
	    Vector3f(2,6,0),
	    Vector3f(2,6,1),
	    Vector3f(2,7,0),
	    Vector3f(2,7,1)
	    );*/


	pos = partEnd;
	growingDir += (grassEnd - partEnd).Normalize() * 0.1;
	growingDir.Normalize();


    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();
}
