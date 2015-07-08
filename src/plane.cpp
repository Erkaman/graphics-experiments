#include "plane.hpp"

#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "math/vector2f.hpp"

#include "common.hpp"

using std::vector;

Plane::Plane(const Vector3f& position): m_position(position){

    m_noiseShader = std::make_unique<ShaderProgram>("shader/noise");
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

    const float SCALE = 10.0f;
    // compute the 4 corners.
    Vector3f c4 = Vector3f(0,0,SCALE);
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


    // create textures.
    CreateTextures();

}

void Plane::CreateTextures() {

    GLfloat* data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 256 * 4);

    int i = 0;

    for(int x = 0; x < 256; x++){
	for(int y = 0; y < 256; y++){
	    int A =  GetPermvalue(x) + y;
            int AA = GetPermvalue(A);
            int AB = GetPermvalue(A + 1);
            int B =  GetPermvalue(x + 1) + y;
            int BA = GetPermvalue(B);
            int BB = GetPermvalue(B + 1);

	    data[i++] = (float)AA/255.0f;
	    data[i++] = (float)AB/255.0f;
	    data[i++] = (float)BA/255.0f;
	    data[i++] = (float)BB/255.0f;

	}
    }

    permutationTable = std::make_unique<Texture2D>((GLvoid* )data, 256, 256, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    permutationTable->Bind();

    permutationTable->SetTextureTiling();
    permutationTable->SetMinFilter(GL_LINEAR);
    permutationTable->SetMagFilter(GL_NEAREST);

    permutationTable->Unbind();







    data = (GLfloat*)malloc(sizeof(GLfloat) * 256 * 1 * 4);

    i = 0;

    for(int x = 0; x < 256; x++){
	int y = 0;

	data[i++] = grads[permutationArray[x] % 16][0];
	data[i++] = grads[permutationArray[x] % 16][1];
	data[i++] = grads[permutationArray[x] % 16][2];
	data[i++] = 1.0f;
    }

    gradTable = std::make_unique<Texture2D>((GLvoid* )data, 256, 1, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    free(data);

    gradTable->Bind();

    gradTable->SetTextureTiling();
    gradTable->SetMinFilter(GL_LINEAR);
    gradTable->SetMagFilter(GL_NEAREST);

    gradTable->Unbind();



}

void Plane::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_noiseShader->Bind();

/*    m_phongShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
*/
    // setup matrices.

    m_noiseShader->SetPhongUniforms(Matrix4f::CreateTranslation(m_position) , camera, lightPosition);

    m_noiseShader->SetUniform("permSampler2d", 0);

    m_noiseShader->SetUniform("permGradSampler", 1);


    Texture::SetActiveTextureUnit(0);
    permutationTable->Bind();

    Texture::SetActiveTextureUnit(1);
    gradTable->Bind();


/*    m_noiseShader->SetUniform("permSampler2d", 0);
    Texture::SetActiveTextureUnit(1);
*/

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);


    permutationTable->Unbind();
    gradTable->Unbind();


    m_noiseShader->Unbind();
}


int Plane::GetPermvalue(int i){
    return permutationArray[i % 256];
}
