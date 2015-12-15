
#include "height_map.hpp"

#include "log.hpp"
#include "perlin_seed.hpp"
#include "common.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "icamera.hpp"
#include "file.hpp"
#include "resource_manager.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/vector2i.hpp"
#include "math/color.hpp"

#include <vector>

#include "lodepng.h"

using std::vector;
using std::string;

constexpr int WIDTH = 256;
constexpr int HEIGHT = 256;
constexpr float SCALE_XZ = 0.8f;

constexpr char VERTEX_FILE[] = "dat/vert.dat";
constexpr char INDEX_FILE[] = "dat/indx.dat";

static inline float Lerp(float x, float y, float a) {
    return x*(1-a)+y*a;
}

static inline int Floor(float x) {
    return x>=0 ? (int)x : (int)x-1;
}


static inline Vector2f Fade(const float px, const float py) {
    return Vector2f(
	px * px * px * (px * (px * 6 - 15) + 10),
	py * py * py * (py * (py * 6 - 15) + 10));
}


static Vector3f CalculateNormal (float north, float south, float east, float west)
{
    Vector3f n(
	(west - east),
	2.0f,
	(north - south));
    return n.Normalize();
}

static Texture* LoadTexture(const string& filename) {
    Texture* texture = Texture2D::Load(filename);

    if(!texture) {
	PrintErrorExit();
    }

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    return texture;
}

HeightMap::HeightMap(const std::string& path): m_isWireframe(false), m_movement(3.0f) {

    m_grassTexture = LoadTexture("img/grass.png");

    m_sandTexture = LoadTexture("img/sand.png");

    m_snowTexture = LoadTexture("img/snow.png");

    m_heightMap =LoadTexture("img/combined.png");

    /*
      load the shader
    */
    m_shader = ShaderProgram::Load("shader/height_map");

    m_depthShader = ShaderProgram::Load("shader/output_depth");


//    if(! (File::Exists(VERTEX_FILE) && File::Exists(INDEX_FILE) )) {
	CreateHeightmap(path);
//    }


    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,3,2} // pos, normal, tex
	);

    size_t dataSize;
    void* vertexData = File::ReadArray(VERTEX_FILE, dataSize);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(dataSize, vertexData);
    m_vertexBuffer->Unbind();
//    free(vertexData);

    m_map = new MultArray<Cell>(WIDTH, HEIGHT, vertexData);


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
    MY_DELETE(m_map)

}


void HeightMap::Render() {

    // setup vertex buffers.
    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_indexBuffer->Bind();

    // DRAW.
    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();

}

void HeightMap::RenderShadowMap(const ICamera* camera) {
    m_depthShader->Bind();

    const Matrix4f mvp = camera->GetMvp(Matrix4f::CreateTranslation(0,0,0));
    m_depthShader->SetUniform("mvp", mvp  );

    Render();

    m_depthShader->Unbind();

}

void HeightMap::Render(const ICamera* camera, const Vector4f& lightPosition) {

    m_shader->Bind();

    m_shader->SetPhongUniforms(Matrix4f::CreateTranslation(0,0,0), camera, lightPosition, Matrix4f::CreateIdentity());

    m_shader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    m_shader->SetUniform("sand", 1);
    Texture::SetActiveTextureUnit(1);
    m_sandTexture->Bind();

    m_shader->SetUniform("snow", 2);
    Texture::SetActiveTextureUnit(2);
    m_snowTexture->Bind();

    m_shader->SetUniform("heightMap", 3);
    Texture::SetActiveTextureUnit(3);
    m_heightMap->Bind();


    // set textures and stuff.


    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    Render();


    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_grassTexture->Unbind();
    m_sandTexture->Unbind();
    m_snowTexture->Unbind();
    m_heightMap->Unbind();


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
    return SCALE_XZ * x;
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

    string* fullPath = ResourceManager::GetInstance().SearchResource(path);
    if(!fullPath) {
	PrintErrorExit();
    }

    lodepng::load_file(buffer, *fullPath);

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

    for(size_t i = 0; i < imageData.size(); i+=4) {

	Cell& c = map(xpos, zpos);

//	float height = imageData[i+1] / 20.0f;

//	LOG_I("red: %d", imageData[i]);

	c.position = Vector3f((float)xpos / (float)width, 0, (float)zpos / (float)depth);

	++xpos;
	if(xpos != 0 && ( xpos % (width) == 0)) {
	    xpos = 0;
	    ++zpos;
	}
    }

/*
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

*/

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

float HeightMap::GetHeightAt(float x, float z)const {

    Vector2f p = Vector2f( ((float)x / SCALE_XZ), ((float)z / SCALE_XZ) );

    Vector2i P =  Vector2i( Floor(p.x),  Floor(p.y) );

    const Vector2f f = Fade(
	p.x - Floor(p.x),
	p.y - Floor(p.y));



    float AA = ((*m_map)(  P.x, P.y )).position.y;
    float AB = ((*m_map)(  P.x, P.y+1 )).position.y;
    float BA = ((*m_map)(  P.x+1, P.y )).position.y;
    float BB = ((*m_map)(  P.x+1, P.y+1 )).position.y;


    return Lerp(Lerp(AA, BA, f.x), Lerp(AB, BB, f.x), f.y);

}
