#include "sphere.hpp"

#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"
#include "gl/shader_program.hpp"

#include "common.hpp"

using std::vector;

Sphere::Sphere() {

    /*
      load the shader
    */

    m_shader = new ShaderProgram("shader/phong");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,

		VBO_NORMAL_ATTRIB_INDEX,
		VBO_COLOR_ATTRIB_INDEX },
	vector<GLuint>{3,3,4}
	);

    m_vertexBuffer->Bind();
//    m_vertexBuffer->SetBufferData(dataSize, vertexData);
    m_vertexBuffer->Unbind();


    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_INT);
//    m_numTriangles = dataSize / (3 * sizeof(GLushort));

    m_indexBuffer->Bind();
//    m_indexBuffer->SetBufferData(dataSize, indexData);
    m_indexBuffer->Unbind();

}

Sphere::~Sphere() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);

    MY_DELETE(m_indexBuffer);
}
