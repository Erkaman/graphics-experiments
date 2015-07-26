
#include "height_map.hpp"

#include "log.hpp"
#include "perlin_seed.hpp"
#include "common.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "camera.hpp"
#include "file.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/color.hpp"
#include "mult_array.hpp"

#include <vector>

#include "lodepng.h"

using std::vector;

constexpr char VERTEX_FILE[] = "dat/vert.dat";
constexpr char INDEX_FILE[] = "dat/indx.dat";

struct Cell {
    Vector3f position;
    Vector3f normal;
//    Color color;
    Vector2f texCoord;
};

static Vector3f CalculateNormal (float north, float south, float east, float west)
{
    Vector3f n(
	(west - east),
	2.0f,
	(north - south));
    return n.Normalize();
}

HeightMap::HeightMap(const std::string& path): m_isWireframe(false), m_movement(3.0f) {

    m_grassTexture = new Texture2D("img/grass.png");

    m_grassTexture->Bind();
    m_grassTexture->SetTextureRepeat();
    m_grassTexture->GenerateMipmap();
    m_grassTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_grassTexture->SetMagFilter(GL_LINEAR);
    m_grassTexture->Unbind();

    m_sandTexture = new Texture2D("img/sand.png");

    m_sandTexture->Bind();
    m_sandTexture->SetTextureRepeat();
    m_sandTexture->GenerateMipmap();
    m_sandTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_sandTexture->SetMagFilter(GL_LINEAR);
    m_sandTexture->Unbind();



    m_snowTexture = new Texture2D("img/snow.png");

    m_snowTexture->Bind();
    m_snowTexture->SetTextureRepeat();
    m_snowTexture->GenerateMipmap();
    m_snowTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_snowTexture->SetMagFilter(GL_LINEAR);
    m_snowTexture->Unbind();


    /*
      load the shader
    */
    m_shader = new ShaderProgram("shader/height_map");


//    if(! (File::Exists(VERTEX_FILE) && File::Exists(INDEX_FILE) )) {
	CreateHeightmap(path);
//    }


    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_NORMAL_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX},
	vector<GLuint>{3,3,2}
	);

    size_t dataSize;
    void* vertexData = File::ReadArray(VERTEX_FILE, dataSize);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(dataSize, vertexData);
    m_vertexBuffer->Unbind();
    free(vertexData);


    void* indexData = File::ReadArray(INDEX_FILE, dataSize);

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_INT);
    m_numTriangles = dataSize / (3 * sizeof(GLushort));

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(dataSize, indexData);
    m_indexBuffer->Unbind();
    free(indexData);
}

HeightMap::~HeightMap() {
    MY_DELETE(m_shader);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_grassTexture);

}


void HeightMap::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_shader->Bind();

    m_shader->SetPhongUniforms(Matrix4f::CreateTranslation(0,0,0), camera, lightPosition);

    m_shader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    m_shader->SetUniform("sand", 1);
    Texture::SetActiveTextureUnit(1);
    m_sandTexture->Bind();

    m_shader->SetUniform("snow", 2);
    Texture::SetActiveTextureUnit(2);
    m_snowTexture->Bind();



    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // setup vertex buffers.
    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_indexBuffer->Bind();

    // DRAW.
    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();


    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_grassTexture->Unbind();
    m_sandTexture->Unbind();
    m_snowTexture->Unbind();


    m_shader->Unbind();
}

const float HeightMap::ComputeY(const unsigned char heightMapData ) {
//    return ((float)heightMapData  / 255.0f) * 0.2;
    return ((float)heightMapData  / 255.0f) * 1.0f;
}

void HeightMap::SetWireframe(const bool wireframe) {
    m_isWireframe = wireframe;
}

const float HeightMap::ScaleXZ(const int x) {
//    return 0.03f * x;
    return 0.3f * x;
}

const Color HeightMap::VertexColoring(const float y) {

    if(y < 1.7f) {
	Color lower = Color::FromInt(237, 201, 175);
	Color higher = Color::FromInt(0x76, 0xa9, 0x12);
	return Color::Lerp(lower, higher, y / 1.7f);
    } else {
	Color lower = Color::FromInt(0x76, 0xa9, 0x12);
	Color higher = Color::FromInt(255, 255 , 255);

	return Color::Lerp(lower, higher, (y-1.7f) / 1.2f);
    }

}

void HeightMap::CreateHeightmap(const std::string& path) {


    /*
      Load the heightmap data.
    */

    std::vector<unsigned char> buffer;
    lodepng::load_file(buffer, path);

    lodepng::State state;
    std::vector<unsigned char> imageData;
    unsigned int width;
    unsigned int depth;

    unsigned error = lodepng::decode(imageData, width, depth, state, buffer);

    if(error != 0){
	LOG_E("could not load height map: %s", lodepng_error_text(error));
    }



    /*
      Next we create the vertex buffer.
    */


    MultArray<Cell> map(width, depth);

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    float high = -9999.0f;
    float low = 9999.0f;

    for(size_t i = 0; i < imageData.size(); i+=4) {

	Cell& c = map(xpos, zpos);

	float height = imageData[i+1] / 20.0f;

//	LOG_I("red: %d", imageData[i]);

	c.position = Vector3f(ScaleXZ(xpos), height, ScaleXZ(zpos));

	high = std::max(c.position.y, high);
	low = std::min(c.position.y, low);

	++xpos;
	if(xpos != 0 && ( xpos % (width) == 0)) {
	    xpos = 0;
	    ++zpos;
	}
    }

    // normalize the vertex data.
    for(size_t x = 0; x < width; ++x) {
	for(size_t z = 0; z < depth; ++z) {
	    Cell& c = map(x,z);

	    c.position.y = (c.position.y - low) / (high - low);
	    c.position.y *=2.7f;
	    c.position.y -=5.0f;

	}
    }

    constexpr int BLEND_RANGE = 1;

    for(size_t x = 0; x < width; ++x) {
	for(size_t z = 0; z < depth; ++z) {
	    Cell& c = map(x,z);


	    float smooth = 0.0f;
	    int samples = 0;

	    for (int xx = -BLEND_RANGE; xx <= BLEND_RANGE; xx++) {
		for(int zz = -BLEND_RANGE; zz <= BLEND_RANGE; ++zz) {
		    smooth += map.GetWrap(x+xx,z+zz).position.y;
		    ++samples;
		}
	    }

	    c.position.y = (smooth / (float)samples);
	}
    }

    float miny = 9999;
    float maxy = -9999;


    for(size_t x = 0; x < width; ++x) {
	for(size_t z = 0; z < depth; ++z) {
	    Cell& c = map(x,z);

	    //  c.position.y *= 2.0f;

	    c.normal = CalculateNormal(
		map.GetWrap(x,z-1).position.y,
		map.GetWrap(x,z+1).position.y,
		map.GetWrap(x+1,z).position.y,
		map.GetWrap(x-1,z).position.y);

//	    c.color = VertexColoring(c.position.y);

//	    LOG_I("pos: %f", c.position.y);

	    c.texCoord.x = (float)x/5.0f;
	    c.texCoord.y = (float)z/5.0f;


	    float y = c.position.y;

	    if(y > maxy) {
		maxy = y;
	    }

	    if(y < miny) {
		miny = y;
	    }


	}
    }

//    LOG_I("min max %f, %f", miny, maxy);


    File::WriteArray(VERTEX_FILE, reinterpret_cast<void *>(map.GetData()), map.GetTotalsize() * sizeof(Cell) );



    GLuint baseIndex = 0;
    UintVector indices;


    for(size_t x = 0; x < (width-1); ++x) {
	for(size_t z = 0; z < (depth-1); ++z) {

	    indices.push_back(baseIndex+width);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+0);

	    indices.push_back(baseIndex+width+1);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+width);

	    baseIndex += 1;
	}
	baseIndex += 1;
    }

    File::WriteArray(INDEX_FILE, reinterpret_cast<void *>(&indices[0]), indices.size() * sizeof(GLuint) );
}
