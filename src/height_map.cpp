#include "height_map.hpp"

#include <vector>
#include "lodepng.h"

#include "log.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "math/matrix4f.hpp"

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

    vertexBuffer = unique_ptr<VBO>(VBO::CreatePosition(3));

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

	const float xpos1 = xpos;
	const float ypos1 = ComputeY(imageData[i]);
	const float zpos1 = zpos;

	const float xpos2 = xpos+1;
	const float ypos2 = ComputeY(imageData[i+4]);
	const float zpos2 = zpos;

	const float xpos3 = xpos;
	const float ypos3 = ComputeY(imageData[i+m_width*4]);
	const float zpos3 = zpos+1;

	const float xpos4 = xpos+1;
	const float ypos4 = ComputeY(imageData[i+m_width*4+4]);
	const float zpos4 = zpos+1;



/*	LOG_I("%f", ypos1*255.0f);
	LOG_I("%f", ypos2*255.0f);
	LOG_I("%f", ypos3*255.0f);
	LOG_I("%f\n", ypos4*255.0f);
*/

	vertices.push_back(xpos1);
	vertices.push_back(ypos1);
	vertices.push_back(zpos1);

	vertices.push_back(xpos2);
	vertices.push_back(ypos2);
	vertices.push_back(zpos2);

	vertices.push_back(xpos3);
	vertices.push_back(ypos3);
	vertices.push_back(zpos3);

	vertices.push_back(xpos4);
	vertices.push_back(ypos4);
	vertices.push_back(zpos4);

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

void HeightMap::Draw(const Matrix4f& mvp) {


    shader->Bind();
    shader->SetUniform("mvp", mvp);

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    vertexBuffer->Bind();
    vertexBuffer->EnableVertexAttrib();
    vertexBuffer->Bind();

    indexBuffer->Bind();

    indexBuffer->DrawIndices(GL_TRIANGLES, m_numTriangles*3);


    indexBuffer->Unbind();

    vertexBuffer->Bind();
    vertexBuffer->DisableVertexAttrib();
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
