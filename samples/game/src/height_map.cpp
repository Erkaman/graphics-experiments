
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
#include "ewa/random.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/vector2i.hpp"
#include "math/color.hpp"

#include <vector>

#include "lodepng.h"

using std::vector;
using std::string;

/*
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
*/

/*
static Vector3f CalculateNormal (float north, float south, float east, float west)
{
    Vector3f n(
	(west - east),
	2.0f,
	(north - south));
    return n.Normalize();
}
*/

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

//    m_heightMap =LoadTexture("img/combined.png");

    /*
      load the shader
    */
    m_shader = ShaderProgram::Load("shader/height_map");

    m_depthShader = ShaderProgram::Load("shader/output_depth");

    m_idShader = ShaderProgram::Load("shader/output_id");


    CreateHeightmap(path);
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
    m_imageTexture->Bind();

    // set textures and stuff.

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


    Render();


    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_grassTexture->Unbind();
    m_sandTexture->Unbind();
    m_snowTexture->Unbind();

    m_imageTexture->Unbind();


    m_shader->Unbind();
}

void HeightMap::RenderId(const ICamera* camera) {

    m_idShader->Bind();

    m_idShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);

    m_idShader->SetUniform("heightMap", 3);
    Texture::SetActiveTextureUnit(3);
    m_imageTexture->Bind();

    Render();

    m_imageTexture->Unbind();

    m_idShader->Unbind();
}




void HeightMap::SetWireframe(const bool wireframe) {
    m_isWireframe = wireframe;
}

/*
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

}*/

void HeightMap::CreateHeightmap(const std::string& path) {


    size_t width = 256;
    size_t depth = 256;

    m_image = new MultArray<unsigned char>(width, depth, (unsigned char)0);

    MultArray<unsigned char>& image = *m_image;

    Random random(3);

    for(size_t i = 0; i < width; ++i) {

	for(size_t j = 0; j < depth; ++j) {
	    image(i,j) = random.RandomInt(0,80);
	}
    }



    m_imageTexture = new Texture2D(image.GetData(), width, depth,
					    GL_R8, // internal format
					    GL_RED, // format
					    GL_UNSIGNED_BYTE
	);

    m_imageTexture->Bind();
    m_imageTexture->SetTextureRepeat();
    m_imageTexture->GenerateMipmap();
    m_imageTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_imageTexture->SetMagFilter(GL_LINEAR);
    m_imageTexture->Unbind();


    image(20,20) = 255;

    m_imageTexture->Bind();

    m_imageTexture->UpdateTexture(image.GetData());

    m_imageTexture->Unbind();

     m_map = new MultArray<Cell>(width, depth);
     MultArray<Cell> &map = *m_map;

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    int id = 0;

    for(size_t i = 0; i < width*depth; ++i) {

	Cell& c = map(xpos, zpos);

	c.position = Vector3f((float)xpos / (float)width, 0, (float)zpos / (float)depth);

	float scale = 1.0;

	c.id = id++;

	c.texCoord = Vector2f(xpos*scale, zpos*scale);

	++xpos;
	if(xpos != 0 && ( xpos % (width) == 0)) {
	    xpos = 0;
	    ++zpos;
	}

//	LOG_I("x = %d, z = %d", xpos, zpos);
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

//    File::WriteArray(VERTEX_FILE, reinterpret_cast<void *>(map.GetData()), map.GetTotalsize() * sizeof(Cell) );

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,1,2} // pos, id, tex
	);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(map);
    m_vertexBuffer->Unbind();







    GLuint baseIndex = 0;
    UintVector indices;

    m_numTriangles = 0;


    for(size_t x = 0; x < (width-1); ++x) {
	for(size_t z = 0; z < (depth-1); ++z) {

	    indices.push_back(baseIndex+width);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+0);

	    indices.push_back(baseIndex+width+1);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+width);

	    m_numTriangles+=2;

	    baseIndex += 1;
	}
	baseIndex += 1;
    }


    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_INT);

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();


}

float HeightMap::GetHeightAt(float x, float z)const {
    /*
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
    */

    return 0;
}


void HeightMap::Update(const float delta) {
/*
    MultArray<unsigned char>& image = *m_image;

    static int dir = -1;

    unsigned char c = image(20,20);

    if(c >= 255 && dir==1) {
	dir = -1;
    }

    if(c < 1 && dir==-1) {
	dir = +1;
    }

    c += dir;

    image(20,20) = c;

    m_imageTexture->Bind();

    //TODO: methods better exist:
    // http://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
    m_imageTexture->UpdateTexture(image.GetData());

    m_imageTexture->Unbind();
*/
}
