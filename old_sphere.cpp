#include "sphere.hpp"

#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"
#include "gl/shader_program.hpp"

#include "math/vector3f.hpp"
#include "math/color.hpp"


#include "common.hpp"
#include "camera.hpp"

using std::vector;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );

Sphere::Sphere(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)) {

    /*
      load the shader
    */

    m_shader = new ShaderProgram("shader/sphere");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_COLOR_ATTRIB_INDEX },
	vector<GLuint>{3,4}
	);

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_numTriangles = GenerateVertices(radius, slices, stacks, m_vertexBuffer, m_indexBuffer);
}

Sphere::~Sphere() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
}

void Sphere::Draw(const Camera& camera) {

    m_shader->Bind();

    SetDepthTest(false);

    //   GL_C(glDisable (GL_CULL_FACE));


    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);


    /*
    const Matrix4f modelViewMatrix = camera.GetModelViewMatrix(
	GetModelMatrix());

    const Matrix4f mvp = camera.GetMvp(modelViewMatrix);

    m_shader->SetUniform("mvp", mvp);

*/
    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_shader->Unbind();

//    GL_C(glEnable (GL_CULL_FACE));

    SetDepthTest(true);

}


GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    ) {

    FloatVector vertices;
    UshortVector indices;

    int counter = 0;


    // make a half sphere.
    for (int i = 0; i <= stacks; ++i){

	float V   = (float)i / (float) stacks;
	float phi = V * PI/2.0f; // PI/2.0f, means that we're only generating half a sphere.

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

	    Color(x,y,z).Add(vertices);
	}
    }

    LOG_I("vertices size: %f", vertices.size() / 7.0f );

    // next, we make the vertices of the bottom of the half sphere.

    int i = 0;
    for (; i < slices * stacks + slices; ++i){

	indices.push_back (i + slices);
	indices.push_back (i + slices + 1);
	indices.push_back (i);

	indices.push_back (i + 1);
	indices.push_back (i);
	indices.push_back (i + slices + 1);
    }

    Vector3f pos(0,0,0);
    pos.Add(vertices);
    Color(0,0,0).Add(vertices);

    const GLushort originIndex = i + slices + 1;

    for(int j = 0; j < slices; ++j) {

	indices.push_back(originIndex);

	indices.push_back(i+j+1);

	indices.push_back(i+j);
    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    return indices.size();
}
