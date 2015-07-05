#include "plane.hpp"

#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"

#include "math/matrix4f.hpp"
#include "math/vector2f.hpp"

#include "common.hpp"

using std::vector;

Plane::Plane(const Vector3f& position): m_position(position){

    m_noiseShader = std::make_unique<ShaderProgram>("shader/phong");
    LOG_I("noiseeeee");

    m_vertexBuffer = std::unique_ptr<VBO>(VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_NORMAL_ATTRIB_INDEX,
							    VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{3,3,2}
						    ));


    m_indexBuffer = std::unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_SHORT));

    FloatVector vertices;
    UshortVector indices;

    const float SCALE = 1.0f;
    // compute the 4 corners.
    Vector3f c4 = Vector3f(0,0,SCALE);
    Vector3f n = Vector3f(0,1,0);

    Vector3f(0,0,0).Add(vertices);
    n.Add(vertices);
    Vector2f(0,0).Add(vertices);

    Vector3f(SCALE,0,0).Add(vertices);
    n.Add(vertices);
    Vector2f(1,0).Add(vertices);


    Vector3f(SCALE,0,SCALE).Add(vertices);
    n.Add(vertices);
    Vector2f(1,1).Add(vertices);

    Vector3f(0,0,SCALE).Add(vertices);
    n.Add(vertices);
    Vector2f(0,1).Add(vertices);

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

void Plane::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_noiseShader->Bind();

/*    m_phongShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
*/
    // setup matrices.

    m_noiseShader->SetPhongUniforms(Matrix4f::CreateTranslation(m_position) , camera, lightPosition);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);


    m_noiseShader->Unbind();
}
