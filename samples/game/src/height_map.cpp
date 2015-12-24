
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

#include "gui_enum.hpp"
#include "ewa/physics_world.hpp"


#include <vector>

#include "lodepng.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include <btBulletDynamicsCommon.h>

using std::vector;
using std::string;

constexpr unsigned short MAX_HEIGHT = 65535;
constexpr unsigned short MID_HEIGHT = 32768;
constexpr unsigned short MIN_HEIGHT = 0;

//const Vector3f offset(0, 0, 0);

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
					       m_dirtTexture(NULL), m_config(Config::GetInstance()),
					       m_cursorPosition(0,0), m_cursorPositionWasUpdated(true),
					       m_xzScale(100.0),
					       m_yScale(4.0),
					       m_resolution(256)
{


    // position the heightfield so that it is centered at the origin:
    m_offset = Vector3f(-m_xzScale/2.0,0,-m_xzScale/2.0);

    m_grassTexture = LoadTexture("img/grass.png");

    m_dirtTexture = LoadTexture("img/dirt.png");

    m_rockTexture = LoadTexture("img/rock.png");

    /*
      load the shader
    */
    m_shader = ShaderProgram::Load("shader/height_map");

    m_depthShader = ShaderProgram::Load("shader/output_depth");

    m_idShader = ShaderProgram::Load("shader/height_map_output_id");
    m_cursorShader = ShaderProgram::Load("shader/height_map_cursor");

    CreateHeightmap(path);

    CreateSplatMap();

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
		points.push_back(Vector3f((float)ix / (float)m_resolution, 0, (float)iz / (float)m_resolution));
	    }
	}
    }


    m_cursorVertexBuffer->Bind();
    m_cursorVertexBuffer->SetBufferData(points);
    m_cursorVertexBuffer->Unbind();

    m_numCursorPoints = points.size();
}

void HeightMap::RenderSetup(ShaderProgram* shader) {
    shader->SetUniform("heightMap", 3);
    Texture::SetActiveTextureUnit(3);
    m_heightMap->Bind();

    shader->SetUniform("splatMap", 4);
    Texture::SetActiveTextureUnit(4);
    m_splatMap->Bind();



    shader->SetUniform("xzScale", m_xzScale);
    shader->SetUniform("yScale", m_yScale);
    shader->SetUniform("offset", m_offset);
    shader->SetUniform("resolution", (float)m_resolution);
}

void HeightMap::RenderUnsetup(ShaderProgram* shader) {
    m_heightMap->Unbind();
    m_splatMap->Unbind();
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

    m_shader->SetUniform("dirt", 1);
    Texture::SetActiveTextureUnit(1);
    m_dirtTexture->Bind();

    m_shader->SetUniform("rock", 2);
    Texture::SetActiveTextureUnit(2);
    m_rockTexture->Bind();

/*
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
    m_dirtTexture->Unbind();
    m_rockTexture->Unbind();

    m_shader->Unbind();
}


void HeightMap::RenderCursor(const ICamera* camera) {




    m_cursorShader->Bind();

    m_cursorShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);


    m_cursorShader->SetUniform("cursorPosition",
			       Vector3f(m_cursorPosition.x, 0, m_cursorPosition.y) );


    // set point size.
    glPointSize(7.0);

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


    size_t width = m_resolution;
    size_t depth = m_resolution;


    size_t iBuffer = 0;

    MultArray<unsigned short>& heightData = *m_heightData;

    for(size_t j = 0; j < depth; ++j) {

	for(size_t i = 0; i < width; ++i) {

	    unsigned char c1 = buffer[iBuffer+0];
	    unsigned char c2 = buffer[iBuffer+1];

	    unsigned short s = (c1 << 8) | c2;

	    heightData(i,j) = s;

	    iBuffer += 2;

	    if(iBuffer > buffer.size() ) {
		LOG_I("exceeded size: %ld, %ld, %ld", iBuffer, i, j);
		exit(1);
	    }
	}
    }
}

void HeightMap::CreateSplatMap() {

    size_t width = m_resolution;
    size_t depth = m_resolution;

    Random random(3);

    SplatColor def; // default splat color.
    def.r = 255;
    def.g = 0;
    def.b = 0;
    def.a = 0;

    m_splatData = new MultArray<SplatColor>(width, depth, def  );

    MultArray<SplatColor>& splatData = *m_splatData;
/*
    bool firstHalf = true;


    for(size_t i = 0; i < width; ++i) {

	for(size_t j = 0; j < depth; ++j) {

	    if(firstHalf) {
		splatData(i,j).r = 255;
		splatData(i,j).g = 0;
	    } else {
		splatData(i,j).r = 0;
		splatData(i,j).g = 255;
	    }

	}

	if(i > width/2) {
	    firstHalf = false;
	}
    }
*/

    m_splatMap = new Texture2D(splatData.GetData(), width, depth,
				   GL_RGBA8, // internal format
				   GL_RGBA, // format
				   GL_UNSIGNED_BYTE);

    m_splatMap->Bind();
    m_splatMap->SetTextureClamping();
    m_splatMap->GenerateMipmap();
    m_splatMap->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_splatMap->SetMagFilter(GL_LINEAR);
    m_splatMap->Unbind();

//    image(20,20) = 10000;

    m_splatMap->Bind();
    m_splatMap->UpdateTexture(splatData.GetData());
    m_splatMap->Unbind();

}


void HeightMap::CreateHeightmap(const std::string& path) {

    size_t width = m_resolution;
    size_t depth = m_resolution;

    Random random(3);

    m_heightData = new MultArray<unsigned short>(width, depth, (unsigned short)0);

    MultArray<unsigned short>& heightData = *m_heightData;

    for(size_t i = 0; i < width; ++i) {

	for(size_t j = 0; j < depth; ++j) {
	    heightData(i,j) = MIN_HEIGHT;
	}
    }

    m_heightMap = new Texture2D(heightData.GetData(), width, depth,
				   GL_R16, // internal format
				   GL_RED, // format
				   GL_UNSIGNED_SHORT
	);

    m_heightMap->Bind();
    m_heightMap->SetTextureClamping();
    m_heightMap->GenerateMipmap();
    m_heightMap->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_heightMap->SetMagFilter(GL_LINEAR);
    m_heightMap->Unbind();

//    image(20,20) = 10000;

    m_heightMap->Bind();

    m_heightMap->UpdateTexture(heightData.GetData());

    m_heightMap->Unbind();

    m_map = new MultArray<Cell>(width, depth);
    MultArray<Cell> &map = *m_map;

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    int id = 0;

    for(size_t i = 0; i < width*depth; ++i) {

	Cell& c = map(xpos, zpos);

	float x = (float)xpos / (float)width;
	float z = (float)zpos / (float)depth;

	c.position =
	    Vector3f(
		x,
		0,
		z
		);

	c.id = id++;

	c.texCoord = Vector2f(x,z);

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
    Vector3f x0(m_offset); // plane offset
    Vector3f o = rayOrigin;
    Vector3f d = rayDir;

    float t = (Vector3f::Dot(n, x0) - Vector3f::Dot(n, o)) / (Vector3f::Dot(n, d));

    if(t < 0) {
	// not hitting plane. do nothing.

    } else {

	// the position where the ray hits the plane.
	Vector3f hit = rayOrigin + rayDir * t;

	// now find which, exact, height position is hit by the ray.

	hit -= m_offset;
	hit = hit * (1.0 / m_xzScale);
	hit = hit * m_resolution;

	int xHit = hit.x;
	int zHit = hit.z;

	if(xHit >= 0 && xHit < (int)m_resolution &&
	   zHit >= 0 && zHit < (int)m_resolution) {
	    // if cursor is actually hitting the plane, update cursor position.
//	    LOG_I("UPDATE cursor: %d, %d", xHit, zHit);

	    if(xHit != m_cursorPosition.x && zHit != m_cursorPosition.y) {
		m_cursorPosition.x = xHit;
		m_cursorPosition.y = zHit;
		m_cursorPositionWasUpdated = true;
	    } else {
	    }




	}
    }
}




void HeightMap::ModifyTerrain(const float delta) {

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& image = *m_heightData;

    float rad = 35;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;


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

	m_heightMap->Bind();

	//TODO: methods better exist:
	// http://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
	m_heightMap->UpdateTexture(image.GetData());

	m_heightMap->Unbind();

	++istep;
    }
}

void HeightMap::DrawTexture(const float delta, int drawTextureType) {

    MultArray<SplatColor>& splatData = *m_splatData;

    float rad = 35;

    float fadeRad = 30;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    if(m_cursorPositionWasUpdated) {

	// we need to move cursor again to draw again.
	m_cursorPositionWasUpdated = false;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int px = cx+ix;
		int pz = cz+iz;

		if(px < 0 || px >= m_resolution || pz < 0 || pz >= m_resolution) {
		    continue;
		}

		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		if(dist <= rad) {

		    unsigned char val;

		    if(dist > fadeRad) {
			float f1 = dist - fadeRad;
			float f2 = rad - fadeRad;

			val = (unsigned char)((1.0 - f1/f2) * 255.0);

		    } else {
			val = 255;
		    }


		    if(drawTextureType != EraserTexture) {


			unsigned char* textures =  (unsigned char *)&splatData(px,pz);

			// find the texture that has the most influence on the pixel.
			// we will blend between this texture and the current texture.
			int imax = drawTextureType == GrassTexture ? 1 : 0;
			for(int i = imax+1; i < 4; ++i) {
			    if(textures[i] > textures[imax] && i != drawTextureType) {
				imax = i;
			    }
			}

			if(imax == drawTextureType) {
			    // this texture is already drawn on this pixel. Do nothing.
			    continue;
			}

			unsigned char& current = textures[drawTextureType];

			unsigned char& other = textures[imax];

			if(current < val) {
			    current = val;

			    if(other > 0)

				other = 255 - val;
			}


		    } else {


			SplatColor& color =  splatData(px,pz);
			color.r = 255;
			color.g = 0;
			color.b = 0;
			color.a = 0;


		    }



		}
	    }
	}

	m_splatMap->Bind();
	m_splatMap->UpdateTexture(splatData.GetData());
	m_splatMap->Unbind();

    } else {

    }

}


void HeightMap::Update(const float delta, ICamera* camera,
		       const float framebufferWidth,
		       const float framebufferHeight) {

    if(m_config.IsGui()) {
	UpdateCursor(camera,framebufferWidth, framebufferHeight);
    }
}

void HeightMap::SaveHeightMap() {
 	    m_heightMap->Write16ToFile("height.png");

}


void HeightMap::SaveSplatMap() {
    m_splatMap->WriteToFile("splat.png");
}


//the bad lightning may be caused beccause the transition from hill to grass is very bad.

/*

btHeightfieldTerrainShape::btHeightfieldTerrainShape	(
    int 	heightStickWidth, // number of squares in heightmap. 256 in our case.
    int 	heightStickLength, // see above
    const void * 	heightfieldData, // byte(?)  sequence of the heightmap.  data.
    btScalar 	heightScale, // not sure...
    btScalar 	minHeight, // max height in float. should be 1 in our case, since this is the true range.
btScalar 	maxHeight,

    int 	upAxis, // should be 1, if y-axis is up.
PHY_ScalarType 	heightDataType,
bool 	flipQuadEdges
)

*/

void HeightMap::AddToPhysicsWorld(PhysicsWorld* physicsWorld) {

    // get new heightfield of appropriate type

    unsigned short* rawHeightMap = m_heightMap->GetPixels<unsigned short>(
	m_resolution* m_resolution * 1, GL_RED, GL_UNSIGNED_SHORT
	);

    // create a signed height map.
    unsigned char* signedRawHeightMap = new unsigned char[m_resolution* m_resolution *2];

    for(size_t i = 0; i < m_resolution* m_resolution; ++i) {

	unsigned short us = rawHeightMap[i];

	signed short ss =
	    us >= MID_HEIGHT ?
	    us - MID_HEIGHT :
	    MID_HEIGHT -us;

	unsigned char c1 = (ss >> 8) & 255;
	unsigned char c2 = ss & 255;

	*signedRawHeightMap = c1;
	++signedRawHeightMap;
	*signedRawHeightMap = c2;
	++signedRawHeightMap;

//	signedRawHeightMap[i] = ss;

//	LOG_I("lol: %d", signedRawHeightMap[i] );
    }


/*
    LOG_I("raw: %d",  rawHeightMap[0] );
    LOG_I("raw: %d",  rawHeightMap[1] );
    LOG_I("raw: %d",  rawHeightMap[2] );

*/

	bool flipQuadEdges = false;
	btHeightfieldTerrainShape * heightfieldShape =
	    new btHeightfieldTerrainShape(m_resolution, m_resolution,
					  rawHeightMap,
					  m_yScale / 32767.0,  //s_gridHeightScale,
					  -4, // min height
					  +4, // max height
					  1, // y-axis is up.
					  PHY_SHORT,
					  flipQuadEdges);


	btAssert(heightfieldShape && "null heightfield");

	// scale the shape
//	btVector3 localScaling = getUpVector(m_upAxis, s_gridSpacing, 1.0);
	heightfieldShape->setLocalScaling(
	    btVector3(100.0f/(float)m_resolution,1, 100.0f/(float)m_resolution)
	    );

	// set origin to middle of heightfield
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,-4,0));

	// create ground object
	float mass = 0.0;

	btVector3 inertia(0, 0, 0);


	btMotionState* motionState = new btDefaultMotionState(tr);

	btRigidBody::btRigidBodyConstructionInfo ci(mass, motionState, heightfieldShape, inertia);

	btRigidBody* rigidBody = new btRigidBody(ci);

	physicsWorld->AddRigidBody(rigidBody);
//	localCreateRigidBody(mass, tr, heightfieldShape);



}
