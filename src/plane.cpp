#include "plane.hpp"
#include "perlin_seed.hpp"

#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "math/vector2f.hpp"

using std::vector;

Plane::Plane(const Vector3f& position): m_position(position){

    m_noiseShader = new ShaderProgram("shader/noise");

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

    m_perlinSeed = new PerlinSeed(9);
}


void Plane::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_noiseShader->Bind();

    m_noiseShader->SetPhongUniforms(Matrix4f::CreateTranslation(m_position) , camera, lightPosition);

    m_perlinSeed->Bind(*m_noiseShader);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_perlinSeed->Unbind();

    m_noiseShader->Unbind();
}

Plane::~Plane() {
    delete m_perlinSeed;
    delete m_vertexBuffer;
    delete m_indexBuffer;
    delete m_noiseShader;
}
