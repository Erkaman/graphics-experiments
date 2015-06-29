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
#include "math/color.hpp"

//#include "common.hpp"

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
				       vector<GLuint>{
					   VBO_POSITION_ATTRIB_INDEX,
					       VBO_NORMAL_ATTRIB_INDEX,
					       VBO_COLOR_ATTRIB_INDEX},
				       vector<GLuint>{3,3,4}
				       ));


    FloatVector vertices;
    UintVector indices;

    GLuint baseIndex = 0;

    m_numTriangles = 0;

    LOG_I("width: %d", m_width);
    LOG_I("depth: %d", m_depth);
    LOG_I("imageData size: %ld", imageData.size());

    while(true) {

	if(xpos != 0 && ( (xpos+1) % (m_width) == 0)) {
	    ++zpos;
	    xpos = 0;
	}

	if((zpos+1)==m_depth) {
	    LOG_I("break %d", zpos);
	    break;
	}

	size_t i = ( zpos * (m_width) + xpos) * 4;

	const Vector3f v1(ScaleXZ(xpos), ComputeY(imageData[i]), ScaleXZ(zpos));

	const Vector3f v2(ScaleXZ(xpos+1), ComputeY(imageData[i+4]), ScaleXZ(zpos));

	const Vector3f v3(ScaleXZ(xpos), ComputeY(imageData[i+m_width*4]), ScaleXZ(zpos+1));

	const Vector3f v4(ScaleXZ(xpos+1), ComputeY(imageData[i+m_width*4+4]), ScaleXZ(zpos+1));


/*	if(i > 67000)
	    LOG_I("index: %ld, %d, v1=%s, v4=%s", i+m_width*4+4, imageData[i+m_width*4+4], tos(v1).c_str(), tos(v4).c_str() );
*/
	const Vector3f normal = Vector3f::Cross(v3 - v1, v2 - v1).Normalize();

	v1.Add(vertices);
	normal.Add(vertices);
	VertexColoring(v1.y).Add(vertices);

	v2.Add(vertices);
	normal.Add(vertices);
	VertexColoring(v2.y).Add(vertices);

	v3.Add(vertices);
	normal.Add(vertices);
	VertexColoring(v3.y).Add(vertices);

	v4.Add(vertices);
	normal.Add(vertices);
	VertexColoring(v4.y).Add(vertices);

	indices.push_back(2+baseIndex);
	indices.push_back(1+baseIndex);
	indices.push_back(0+baseIndex);

	indices.push_back(1+baseIndex);
	indices.push_back(2+baseIndex);
	indices.push_back(3+baseIndex);

	m_numTriangles += 2;
	baseIndex += 4;
//	LOG_I("base index: %d", baseIndex);

	++xpos;

    }

    LOG_I("base index: %d", baseIndex);


    LOG_I("size of vertex data: %ld", vertices.size());
    LOG_I("size of index data: %ld", indices.size());
/*
  every square takes 6 indices.

  number of indices is 99846
 */
    vertexBuffer->Bind();
    vertexBuffer->SetBufferData(vertices);
    vertexBuffer->Unbind();

    /*
      Finally, we create the index buffer.
     */
    indexBuffer = unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_INT));

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

    indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    indexBuffer->Unbind();

    vertexBuffer->Bind();
    vertexBuffer->DisableVertexAttribInterleaved();
    vertexBuffer->Bind();

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    shader->Unbind();
}

const float HeightMap::ComputeY(const unsigned char heightMapData ) {
//    return ((float)heightMapData  / 255.0f) * 0.2;
    return ((float)heightMapData  / 255.0f) * 1.0;

}

void HeightMap::SetWireframe(const bool wireframe) {
    m_isWireframe = wireframe;
}

const float HeightMap::ScaleXZ(const float x) {
//    return 0.03f * x;
    return x;

}

const Color HeightMap::VertexColoring(const float y) {
    return Color(0.33,0.33,0.33);
    /*
    if(y < 0.5f) {
	Color lower = Color::FromInt(237, 201, 175);
	Color higher = Color::FromInt(0, 255, 0);
	return Color::Lerp(lower, higher, y / 0.5f);
    } else {
	Color lower = Color::FromInt(0, 255, 0);
	Color higher = Color::FromInt(100, 100, 100);
	return Color::Lerp(lower, higher, (y-0.5f) / 0.5f);
	}*/
}
