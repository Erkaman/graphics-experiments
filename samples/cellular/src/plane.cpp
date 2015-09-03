#include "plane.hpp"

#include "ewa/perlin_seed.hpp"
#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

using std::vector;

Plane::Plane(const Vector3f& position, const bool isCellular ): m_modelMatrix(Matrix4f::CreateTranslation(position)){

    m_shader = isCellular ? new ShaderProgram("shader/cellular") : new ShaderProgram("shader/perlin");

    GeometryObjectData data;

    data.m_vertexAttribsSizes = vector<GLuint>{3,3,2};
    data.m_indexType = GL_UNSIGNED_SHORT;

    FloatVector& vertices= data.m_vertices;
    UshortVector& indices = data.m_indices;

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


    data.m_numTriangles = 2;

    m_perlinSeed = new PerlinSeed(1);

    GeometryObject::Init(data);


}

void Plane::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_shader->Bind();

    m_perlinSeed->Bind(*m_shader);


    m_shader->SetPhongUniforms(

	m_modelMatrix
	 , camera, lightPosition);


    GeometryObject::Render();



    m_perlinSeed->Unbind();


    m_shader->Unbind();
}

Plane::~Plane() {
    MY_DELETE(m_shader);
    MY_DELETE(m_perlinSeed);
}
