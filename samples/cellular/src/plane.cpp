#include "plane.hpp"

#include "ewa/perlin_seed.hpp"
#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

using std::vector;

Plane::Plane(const Vector3f& position, const Vector3f& scale,const bool isCellular ): GeometryObject(position, scale){

    m_shader = isCellular ? new ShaderProgram("shader/cellular") : new ShaderProgram("shader/perlin");


    m_vertexBuffer = VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_NORMAL_ATTRIB_INDEX,
							    VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{3,3,2}
			  );


    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    FloatVector vertices;
    UshortVector indices;

    const float SCALE = 10.0f;
    // compute the 4 corners.
    Vector3f n = Vector3f(0,1,0);

    Vector3f(0,0,0).Add(vertices);
    n.Add(vertices);
    Vector2f(0,0).Add(vertices);

    Vector3f(SCALE,0,0).Add(vertices);
    n.Add(vertices);
    Vector2f(SCALE,0).Add(vertices);


    Vector3f(SCALE,0,SCALE).Add(vertices);
    n.Add(vertices);
    Vector2f(SCALE,SCALE).Add(vertices);

    Vector3f(0,0,SCALE).Add(vertices);
    n.Add(vertices);
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

    m_perlinSeed = new PerlinSeed(1);

}

void Plane::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_shader->Bind();

    m_perlinSeed->Bind(*m_shader);


    m_shader->SetPhongUniforms(

	GetModelMatrix()
	 , camera, lightPosition);


    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_perlinSeed->Unbind();


    m_shader->Unbind();
}

Plane::~Plane() {
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_shader);
    MY_DELETE(m_perlinSeed);
}
