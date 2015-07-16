#include "sphere.hpp"

#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"
#include "gl/shader_program.hpp"

#include "math/vector3f.hpp"

#include "value_noise_seed.hpp"

#include "common.hpp"
#include "camera.hpp"

using std::vector;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );

Sphere::Sphere(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)), m_delta(0) {

    /*
      load the shader
    */

    m_shader = new ShaderProgram("shader/sphere");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,3}
	);

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_numTriangles = GenerateVertices(radius, slices, stacks, m_vertexBuffer, m_indexBuffer);

    m_perlinSeed = new ValueNoiseSeed(2);

}



Sphere::~Sphere() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_perlinSeed);

}

void Sphere::Draw(const Camera& camera) {

    m_shader->Bind();

    SetDepthTest(false);

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
//    m_shader->SetUniform("sampler", 0);
    m_shader->SetUniform("delta", m_delta);

//    LOG_I("delta: %f", m_delta);

    m_perlinSeed->Bind(*m_shader);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_perlinSeed->Unbind();


    m_shader->Unbind();

    SetDepthTest(true);

}


GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    ) {

    FloatVector vertices;
    UshortVector indices;

    // loop through stacks.
    for (int i = 0; i <= stacks; ++i){

	float V   = (float)i / (float) stacks;
	float phi = V * PI;

	// loop through the slices.
	for (int j = 0; j <= slices; ++j){

	    float U = (float)j / (float) slices;
	    float theta = U * (PI * 2);

	    // use spherical coordinates to calculate the positions.
	    float x = cos (theta) * sin (phi);
	    float y = cos (phi);
	    float z = sin (theta) * sin (phi);

	    Vector3f pos(x,y,z);
	    pos *= radius;
	    pos.Add(vertices);

//	    Color(x,y,z).Add(vertices);
	    Vector3f(x,y,z).Add(vertices);

//	    LOG_I("Y: %f", y);

	}
    }

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){

	indices.push_back (i + slices);
	indices.push_back (i + slices + 1);
	indices.push_back (i);

	indices.push_back (i + 1);
	indices.push_back (i);
	indices.push_back (i + slices + 1);
    }


//    LOG_I("SPHERE");

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    return indices.size();
}

void Sphere::Update(const float delta) {
    m_delta += delta;
}
