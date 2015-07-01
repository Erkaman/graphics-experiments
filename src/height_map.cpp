/*
http://www.flipcode.com/archives/Calculating_Vertex_Normals_for_Height_Maps.shtml
http://www.gamedev.net/topic/163625-fast-way-to-calculate-heightmap-normals/

 */


#include "height_map.hpp"

#include "log.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "camera.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/color.hpp"
#include "mult_array.hpp"

#include <memory>
#include <vector>

#include "lodepng.h"

#if defined (_WIN32)
#include <memory>
using std::make_unique;
#endif

using std::unique_ptr;
using std::vector;

struct Cell {
    Vector3f position;
    Vector3f normal;
    Color color;
    Vector2f texCoord;
};

static Vector3f CalculateNormal (float north, float south, float east, float west)
{
    Vector3f n(
	west - east,
	2.0f,
	north - south);

//    LOG_I("nsew: %f, %f, %f, %f", north, south, east, west);

    //  LOG_I("unnormalize  normal: %s", tos(n).c_str() );

	return n.Normalize();
}


HeightMap::HeightMap(const std::string& path): m_isWireframe(false), m_movement(3.0f) {

    m_noiseTexture = make_unique<Texture2D>("img/noise.png");

    m_noiseTexture->Bind();
    m_noiseTexture->SetTextureTiling();
    m_noiseTexture->SetMinFilter(GL_LINEAR);
    m_noiseTexture->SetMagFilter(GL_NEAREST);
    m_noiseTexture->Unbind();



    /*
      load the shader
     */
    m_shader = make_unique<ShaderProgram>("shader/height_map");

    m_normalsShader = make_unique<ShaderProgram>("shader/draw_normals");


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


    m_lightPosition = Vector4f(ScaleXZ(m_width),10,ScaleXZ(m_width), 1.0);

    /*
      Next we create the vertex buffer.
     */


    m_vertexBuffer = unique_ptr<VBO>(VBO::CreateInterleaved(
				       vector<GLuint>{
					   VBO_POSITION_ATTRIB_INDEX,
					       VBO_NORMAL_ATTRIB_INDEX,
					       VBO_COLOR_ATTRIB_INDEX,
					       VBO_TEX_COORD_ATTRIB_INDEX},
				       vector<GLuint>{3,3,4,2}
				       ));

    UintVector indices;


    m_numTriangles = 0;


    MultArray<Cell> map(m_width, m_depth);

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    float high = -9999.0f;
    float low = 9999.0f;

    for(size_t i = 0; i < imageData.size(); i+=4) {

	Cell& c = map.Get(xpos, zpos);

	float height = imageData[i+1] / 20.0f;

//	LOG_I("red: %d", imageData[i]);

	c.position = Vector3f(ScaleXZ(xpos), height, ScaleXZ(zpos));

	high = std::max(c.position.y, high);
	low = std::min(c.position.y, low);

	++xpos;
	if(xpos != 0 && ( xpos % (m_width) == 0)) {
	    xpos = 0;
	    ++zpos;
	}
    }

    // normalize the vertex data.
    for(size_t x = 0; x < m_width; ++x) {
	for(size_t z = 0; z < m_depth; ++z) {
	    Cell& c = map.Get(x,z);

	    c.position.y = (c.position.y - low) / (high - low);


//	    LOG_I("height: %f", c.position.y);

	}
    }

    constexpr int BLEND_RANGE = 0;

    for(size_t x = 0; x < m_width; ++x) {
	for(size_t z = 0; z < m_depth; ++z) {
	    Cell& c = map.Get(x,z);


/*	    float smooth = 0.0f;
	    int samples = 0;

	    for (int xx = -BLEND_RANGE; xx <= BLEND_RANGE; xx++) {
		for(int zz = -BLEND_RANGE; zz <= BLEND_RANGE; ++zz) {
		    smooth += map.GetWrap(x+xx,z+zz).position.y;
		    ++samples;
		}
		}*/

//	    c.position.y *= 2.0f;// (smooth / (float)samples) * 2.0f;
	}
    }


    for(size_t x = 0; x < m_width; ++x) {
	for(size_t z = 0; z < m_depth; ++z) {
	    Cell& c = map.Get(x,z);

	    c.position.y *= 2.0f;

	    c.normal = CalculateNormal(
		map.GetWrap(x,z-1).position.y,
		map.GetWrap(x,z+1).position.y,
		map.GetWrap(x+1,z).position.y,
		map.GetWrap(x-1,z).position.y);

	    if(x == 3 && z == 3)
		LOG_I("normal: %s", tos(c.normal).c_str() );

	    c.color = VertexColoring(c.position.y);

	    c.texCoord.x = x;
	    c.texCoord.y = z;


	}
    }
//    exit(1);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(map);
    m_vertexBuffer->Unbind();

    GLuint baseIndex = 0;

    for(size_t x = 0; x < (m_width-1); ++x) {
	for(size_t z = 0; z < (m_depth-1); ++z) {

	    indices.push_back(baseIndex+m_width);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+0);

	    indices.push_back(baseIndex+m_width+1);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+m_width);

	    m_numTriangles += 2;
	    baseIndex += 1;
	}
	baseIndex += 1;
    }

    m_indexBuffer = unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_INT));


    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();


}


void HeightMap::Draw(const Camera& camera)  {

    m_shader->Bind();

    // setup texture.
    m_noiseTexture->Bind();
    m_shader->SetUniform("noiseSampler", 0);

    // Set up matrices.
    m_shader->SetUniform("mvp", camera.GetMvp());
    Matrix4f viewMatrix = camera.GetViewMatrix();
    m_shader->SetUniform("modelViewMatrix", viewMatrix);
    Matrix4f normalMatrix(viewMatrix);
    normalMatrix.Transpose().Inverse();
    m_shader->SetUniform("normalMatrix", normalMatrix);
    m_shader->SetUniform("viewSpaceLightPosition", Vector3f(viewMatrix * m_lightPosition) );

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // setup vertex buffers.
    m_vertexBuffer->Bind();
    m_vertexBuffer->EnableVertexAttribInterleaved();
    m_vertexBuffer->Bind();

    m_indexBuffer->Bind();

    // DRAW.
    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->Bind();
    m_vertexBuffer->DisableVertexAttribInterleaved();
    m_vertexBuffer->Bind();

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // unsetup texture.
    m_noiseTexture->Unbind();

    m_shader->Unbind();





    m_normalsShader->Bind();



    m_normalsShader->SetUniform("mvp", camera.GetMvp());


    // setup vertex buffers.
    m_vertexBuffer->Bind();
    m_vertexBuffer->EnableVertexAttribInterleaved();
    m_vertexBuffer->Bind();

    m_indexBuffer->Bind();

    // DRAW.
    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->Bind();
    m_vertexBuffer->DisableVertexAttribInterleaved();
    m_vertexBuffer->Bind();


    m_normalsShader->Unbind();

}

const float HeightMap::ComputeY(const unsigned char heightMapData ) {
//    return ((float)heightMapData  / 255.0f) * 0.2;
    return ((float)heightMapData  / 255.0f) * 1.0f;
}

void HeightMap::SetWireframe(const bool wireframe) {
    m_isWireframe = wireframe;
}

const float HeightMap::ScaleXZ(const float x) {
//    return 0.03f * x;
    return 0.2f * x;
}

const Color HeightMap::VertexColoring(const float y) {
//    return Color(0.33f,0.33f,0.33f);

    if(y < 0.7f) {
	Color lower = Color::FromInt(237, 201, 175);
	Color higher = Color::FromInt(0, 255, 0);
	return Color::Lerp(lower, higher, y / 0.7f);
    } else {
	Color lower = Color::FromInt(0, 255, 0);
	Color higher = Color::FromInt(190, 190, 190);
	return Color::Lerp(lower, higher, (y-0.7f) / 0.3f);
    }
}
