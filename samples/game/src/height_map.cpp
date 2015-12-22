
#include "height_map.hpp"

#include "log.hpp"
#include "perlin_seed.hpp"
#include "common.hpp"
#include "gui_mouse_state.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "icamera.hpp"
#include "file.hpp"
#include "resource_manager.hpp"
#include "ewa/random.hpp"
#include "ewa/config.hpp"
#include "ewa/gl/texture_loader.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/vector2i.hpp"
#include "math/color.hpp"

#include <vector>

#include "lodepng.h"

using std::vector;
using std::string;

constexpr float xzScale= 100.0;
const Vector3f offset(50, 0, 50);
constexpr float yScale = 4.0;
constexpr size_t resolution = 256;
constexpr unsigned short MAX_HEIGHT = 65535;

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

HeightMap::HeightMap(const std::string& path): m_isWireframe(false),
					       m_shader(NULL),
					       m_depthShader(NULL),
					       m_idShader(NULL),
					       m_cursorShader(NULL),
					       m_grassTexture(NULL),
					       m_sandTexture(NULL),
					       m_snowTexture(NULL), m_config(Config::GetInstance()){

    m_grassTexture = LoadTexture("img/grass.png");

    m_sandTexture = LoadTexture("img/sand.png");

    m_snowTexture = LoadTexture("img/snow.png");

//    m_heightMap =LoadTexture("img/combined.png");

    /*
      load the shader
    */
    m_shader = ShaderProgram::Load("shader/height_map");

    m_depthShader = ShaderProgram::Load("shader/output_depth");

    m_idShader = ShaderProgram::Load("shader/height_map_output_id");
    m_cursorShader = ShaderProgram::Load("shader/height_map_cursor");

    CreateHeightmap(path);

    CreateCursor();
}

HeightMap::~HeightMap() {


    MY_DELETE(m_shader);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_grassTexture);
    MY_DELETE(m_map)
}

void HeightMap::CreateCursor() {
    vector<Vector3f> points;

    const int rad = 35;

    for(int ix = -rad; ix <= +rad; ++ix) {

	for(int iz = -rad; iz <= +rad; ++iz) {

	    float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

	    if(dist <= rad) {
		points.push_back(Vector3f((float)ix / (float)resolution, 0, (float)iz / (float)resolution));
	    }
	}
    }


    m_cursorVertexBuffer->Bind();
    m_cursorVertexBuffer->SetBufferData(points);
    m_cursorVertexBuffer->Unbind();

    m_numCursorPoints = points.size();
}

void HeightMap::RenderSetup(ShaderProgram* shader) {
    m_shader->SetUniform("heightMap", 3);
    Texture::SetActiveTextureUnit(3);
    m_imageTexture->Bind();


    shader->SetUniform("xzScale", xzScale);
    shader->SetUniform("yScale", yScale);
    shader->SetUniform("offset", offset);
    shader->SetUniform("resolution", (float)resolution);

}

void HeightMap::RenderUnsetup(ShaderProgram* shader) {
    m_imageTexture->Unbind();
}



void HeightMap::Render(ShaderProgram* shader) {

    RenderSetup(shader);


    // setup vertex buffers.
    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_indexBuffer->Bind();

    // DRAW.
    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();


    RenderUnsetup(shader);


}

void HeightMap::RenderShadowMap(const ICamera* camera) {

    // TODO: implement shadow mapping for height map.
/*
    m_depthShader->Bind();

    const Matrix4f mvp = camera->GetMvp(Matrix4f::CreateTranslation(0,0,0));
    m_depthShader->SetUniform("mvp", mvp  );

    Render();

    m_depthShader->Unbind();
*/

}


void HeightMap::RenderHeightMap(const ICamera* camera, const Vector4f& lightPosition) {
    m_shader->Bind();

    m_shader->SetPhongUniforms(Matrix4f::CreateTranslation(0,0,0), camera, lightPosition, Matrix4f::CreateIdentity());

    m_shader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();

/*    m_shader->SetUniform("sand", 1);
    Texture::SetActiveTextureUnit(1);
    m_sandTexture->Bind();

    m_shader->SetUniform("snow", 2);
    Texture::SetActiveTextureUnit(2);
    m_snowTexture->Bind();
*/

    // set textures and stuff.

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    Render(m_shader);

    if(m_isWireframe)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    m_grassTexture->Unbind();
    /*   m_sandTexture->Unbind();
    m_snowTexture->Unbind();
    */

    m_shader->Unbind();
}


void HeightMap::RenderCursor(const ICamera* camera) {




    m_cursorShader->Bind();

    m_cursorShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);
    m_cursorShader->SetUniform("cursorPosition", cursorPosition );

    RenderSetup(m_cursorShader);

    m_cursorVertexBuffer->EnableVertexAttribInterleaved();
    m_cursorVertexBuffer->DrawVertices(GL_POINTS, m_numCursorPoints);
    m_cursorVertexBuffer->DisableVertexAttribInterleaved();


    RenderUnsetup(m_cursorShader);

    m_cursorShader->Unbind();




}


void HeightMap::Render(const ICamera* camera, const Vector4f& lightPosition) {

    RenderHeightMap(camera, lightPosition);

    if(m_config.IsGui()) {
	RenderCursor(camera);
    }
}

void HeightMap::RenderId(const ICamera* camera) {

    m_idShader->Bind();


    m_idShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);

    Render(m_idShader);

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


void HeightMap::LoadHeightmap() {






    std::string* resourcePath = ResourceManager::GetInstance().SearchResource("height.png");
    if(!resourcePath) {
	PrintErrorExit();
    }

    std::vector<unsigned char> tempBuffer;
    lodepng::load_file(tempBuffer,
		       *resourcePath);

    std::vector<unsigned char> buffer;

    unsigned int unused1;
    unsigned int unused2;


    lodepng::State state;
    unsigned error = lodepng::decode(buffer, unused1, unused2/*, state*/, tempBuffer, LCT_GREY, 16);

    if(error != 0){
	SetError("could not load png %s: %s", resourcePath->c_str(), lodepng_error_text(error));
	PrintErrorExit();
    }


    size_t width = resolution;
    size_t depth = resolution;


    size_t iBuffer = 0;

    MultArray<unsigned short>& image = *m_image;

    for(size_t j = 0; j < depth; ++j) {

	for(size_t i = 0; i < width; ++i) {

	    unsigned char c1 = buffer[iBuffer+0];
	    unsigned char c2 = buffer[iBuffer+1];

	    unsigned short s = (c1 << 8) | c2;

	    image(i,j) = s;

	    iBuffer += 2;

	    if(iBuffer > buffer.size() ) {
		LOG_I("exceeded size: %ld, %ld, %ld", iBuffer, i, j);
		exit(1);
	    }

	}
    }


}

void HeightMap::CreateHeightmap(const std::string& path) {

    size_t width = resolution;
    size_t depth = resolution;

    Random random(3);

    m_image = new MultArray<unsigned short>(width, depth, (unsigned short)0);

    MultArray<unsigned short>& image = *m_image;


//    LoadHeightmap();




    for(size_t i = 0; i < width; ++i) {

	for(size_t j = 0; j < depth; ++j) {


//	    image(i,j) = random.RandomInt(0, 20559);
	    image(i,j) = 0;

	}
    }

    m_imageTexture = new Texture2D(image.GetData(), width, depth,
				   GL_R16, // internal format
				   GL_RED, // format
				   GL_UNSIGNED_SHORT
	);

    m_imageTexture->Bind();
    m_imageTexture->SetTextureClamping();
    m_imageTexture->GenerateMipmap();
    m_imageTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_imageTexture->SetMagFilter(GL_LINEAR);
    m_imageTexture->Unbind();

//    image(20,20) = 10000;

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


	c.position =
	    Vector3f(
		(float)xpos / (float)width,
		0,
		(float)zpos / (float)depth);

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

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,1,2} // pos, id, tex
	);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(map);
    m_vertexBuffer->Unbind();

    m_cursorVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3}, // pos
	GL_DYNAMIC_DRAW
	);

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

void HeightMap::UpdateCursor(ICamera* camera,
		       const float framebufferWidth,
		       const float framebufferHeight) {

    float x = GuiMouseState::GetX();
    float y = framebufferHeight -GuiMouseState::GetY()-1;

    // remap cursor position to range [-1,-1]
    // otherwise, we cant use the inverse VP-matrix to recover the world space cursor position
    float remapX = (float)(-1.0f + (2.0f * x) / framebufferWidth);
    float remapY = (float)(-1.0f + (2.0f * y) / framebufferHeight );

    Matrix4f m = camera->GetVp();
    m.Inverse();

    // use inverse VP-matrix to recover cursor position
    Vector4f v =(m * Vector4f(remapX, remapY , 0, 1) );

    v.x /= v.w;
    v.y /= v.w;
    v.z /= v.w;
    v.w = 1.0;

    Vector3f rayOrigin = camera->GetPosition();
    Vector3f rayDir = (Vector3f(v) - rayOrigin  ).Normalize();

//    LOG_I("raydir: %s", tocstr(rayDir) );

    // now raytrace the plane heightmap.

    // plane normal
    Vector3f n(0,1,0);
    Vector3f x0(offset); // plane offset
    Vector3f o = rayOrigin;
    Vector3f d = rayDir;

    float t = (Vector3f::Dot(n, x0) - Vector3f::Dot(n, o)) / (Vector3f::Dot(n, d));

    if(t < 0) {
	// not hitting plane. do nothing.

    } else {


	// the position where the ray hits the plane.
	Vector3f hit = rayOrigin + rayDir * t;

	// now find which, exact, height position is hit by the ray.

	hit -= offset;
	hit = hit * (1.0 / xzScale);
	hit = hit * resolution;

	int xHit = hit.x;
	int zHit = hit.z;

	if(xHit >= 0 && xHit < (int)resolution &&
	   zHit >= 0 && zHit < (int)resolution) {
	    // if cursor is actually hitting the plane, update cursor position.
//	    LOG_I("UPDATE cursor: %d, %d", xHit, zHit);

	    cursorPosition.x = xHit;
	    cursorPosition.y = 0;
	    cursorPosition.z = zHit;


	}
    }
}




void HeightMap::ModifyTerrain(const float delta) {

//    LOG_I("modify");

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& image = *m_image;

    float rad = 35;

    int cx = cursorPosition.x;
    int cz = cursorPosition.z;


    float maxdist = rad;

    static int istep = 0;
    const int max_step = 30;

    if(total > 0.05) {

	total = 0;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		if(dist <= rad) {

		    float x = dist / maxdist; // in range [0,1].

		    float y = (1.0 - x*x);

		    if(y < 0) {
			y = 0;
		    }

		    if(y > 1.0) {
			y = 1.0;
		    }

		    float max =y * (float)MAX_HEIGHT;

		    if( image(cx+ix,cz+iz) < (float)MAX_HEIGHT) {

			float increment = max  / (max_step);


			if(image(cx+ix,cz+iz) + increment > MAX_HEIGHT) {
			    image(cx+ix,cz+iz) = MAX_HEIGHT;

			} else {
			image(cx+ix,cz+iz) += increment;

			}
		    }

//		    istep = max_step+1;

		    //	    if(image(cx+ix,cz+iz) >= 240) {

		}
//		}
	    }
	}

	m_imageTexture->Bind();

	//TODO: methods better exist:
	// http://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
	m_imageTexture->UpdateTexture(image.GetData());

	m_imageTexture->Unbind();

	++istep;

/*
	if(istep > max_step) {


	    m_imageTexture->Write16ToFile("height.png");

	    exit(1);


	}*/
    }
}

void HeightMap::Update(const float delta, ICamera* camera,
		       const float framebufferWidth,
		       const float framebufferHeight) {

    if(m_config.IsGui()) {
	UpdateCursor(camera,framebufferWidth, framebufferHeight);
    }
}

void HeightMap::SaveTexture() {
 	    m_imageTexture->Write16ToFile("height.png");

}
//the bad lightning may be caused beccause the transition from hill to grass is very bad.
