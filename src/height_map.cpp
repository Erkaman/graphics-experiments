#include "height_map.hpp"

#include <vector>
#include "lodepng.h"

#include "log.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "math/matrix4f.hpp"
#include "camera.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

#include "common.hpp"

using std::unique_ptr;
using std::vector;

HeightMap::HeightMap(const std::string& path): m_isWireframe(false) {

    /*
      load the shader
     */
    shader = make_unique<ShaderProgram>("shader/height_map");


    /*
      Load the heightmap data.
     */

    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, path);

    lodepng::State state;
    std::vector<unsigned char> imageData;
    unsigned error = lodepng::decode(imageData, m_width, m_depth, state, buffer);

    if(error != 0){
	LOG_E("could not load height map: %s", lodepng_error_text(error));
    }

    /*
      Next we create the vertex buffer.
     */

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    vertexBuffer = unique_ptr<VBO>(VBO::CreateInterleaved(
				       vector<GLuint>{VBO_POSITION_ATTRIB_INDEX, VBO_NORMAL_ATTRIB_INDEX},
				       vector<GLuint>{3,3}
				       ));


    FloatVector vertices;
    UshortVector indices;

    int baseIndex = 0;

    m_numTriangles = 0;

    while(true) {

	if((zpos+2)==m_depth && (xpos+1) == m_width ) {
	    break;
	}

	if(xpos != 0 && ( (xpos+1) % (m_width) == 0)) {
	    ++zpos;
	    xpos = 0;
	}
	size_t i = (zpos * m_width + xpos) * 4;

//	LOG_I("%ld: %d, %d", i/4, xpos, zpos);

	const Vector3f v1(xpos, ComputeY(imageData[i]), zpos);

	const Vector3f v2(xpos+1, ComputeY(imageData[i+4]), zpos);

	const Vector3f v3(xpos, ComputeY(imageData[i+m_width*4]), zpos+1);

	const Vector3f v4(xpos+1, ComputeY(imageData[i+m_width*4+4]), zpos+1);

	const Vector3f normal = Vector3f::Cross(v3 - v1, v2 - v1);

	v1.Add(vertices);
	normal.Add(vertices);

	v2.Add(vertices);
	normal.Add(vertices);

	v3.Add(vertices);
	normal.Add(vertices);

	v4.Add(vertices);
	normal.Add(vertices);

	indices.push_back(2+baseIndex);
	indices.push_back(1+baseIndex);
	indices.push_back(0+baseIndex);

	indices.push_back(1+baseIndex);
	indices.push_back(2+baseIndex);
	indices.push_back(3+baseIndex);

	m_numTriangles += 2;
	baseIndex += 4;

	++xpos;

    }

    vertexBuffer->Bind();
    vertexBuffer->SetBufferData(vertices);
    vertexBuffer->Unbind();

    /*
      Finally, we create the index buffer.
     */
    indexBuffer = unique_ptr<VBO>(VBO::CreateIndex());

    indexBuffer->Bind();
    indexBuffer->SetBufferData(indices);
    indexBuffer->Unbind();
}


void HeightMap::Draw(const Camera& camera)  {

    shader->Bind();
    shader->SetUniform("mvp", camera.GetMvp());
    Matrix4f viewMatrix = camera.GetViewMatrix();
    shader->SetUniform("modelViewMatrix", viewMatrix);
    Matrix4f normalMatrix(viewMatrix);
    viewMatrix.Transpose().Inverse();
    shader->SetUniform("normalMatrix", normalMatrix);

    Vector4f lightPosition(0,10,0,1);
    shader->SetUniform("viewSpaceLightPosition", viewMatrix * lightPosition);

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    vertexBuffer->Bind();
    vertexBuffer->EnableVertexAttribInterleaved();
    vertexBuffer->Bind();

    indexBuffer->Bind();

    indexBuffer->DrawIndices(GL_TRIANGLES, m_numTriangles*3);

    indexBuffer->Unbind();

    vertexBuffer->Bind();
    vertexBuffer->DisableVertexAttribInterleaved();
    vertexBuffer->Bind();

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    shader->Unbind();
}

const float HeightMap::ComputeY(const unsigned char heightMapData ) {
    return (float)heightMapData  / 255.0f;
}

void HeightMap::SetWireframe(const bool wireframe) {
    m_isWireframe = wireframe;
}
