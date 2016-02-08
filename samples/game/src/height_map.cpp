//todo: add button that rebakes the ao map.
//todo: if the ao map does not exist, we create it at startup. te

#include "height_map.hpp"

#include "log.hpp"
#include "perlin_seed.hpp"
#include "common.hpp"
#include "ewa/gui_mouse_state.hpp"

#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture2d.hpp"

#include "math/math_common.hpp"
#include "math/matrix4f.hpp"
#include "icamera.hpp"
#include "file.hpp"
#include "resource_manager.hpp"
#include "ewa/random.hpp"
#include "ewa/config.hpp"
#include "ewa/gl/texture_loader.hpp"
#include "gl/depth_fbo.hpp"
#include "ewa/cube.hpp"

#include "math/vector2f.hpp"
#include "math/vector3f.hpp"
#include "math/vector4f.hpp"
#include "math/vector2i.hpp"
#include "math/color.hpp"

#include "gui_enum.hpp"
#include "ewa/physics_world.hpp"
#include "value_noise.hpp"
#include "ewa/view_frustum.hpp"


#include "physics_mask.hpp"

#include <vector>

#include "lodepng.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

#include <btBulletDynamicsCommon.h>

using std::vector;
using std::string;

constexpr unsigned short MAX_HEIGHT = 65535;
constexpr unsigned short MID_HEIGHT = 32768;
constexpr unsigned short MIN_HEIGHT = 0;



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

void HeightMap::Init(
    const std::string& heightMapFilename,
    const std::string& splatMapFilename,
    const std::string& aoMapFilename,
    bool guiMode ) {

    m_aabbWireframe = Cube::Load();

    m_isWireframe = false;
    m_shader = NULL;
    m_depthShader = NULL;
//    m_idShader = NULL;
    m_cursorShader = NULL;
    m_grassTexture = NULL;
    m_dirtTexture = NULL;
    m_tempData = NULL;
    m_heightMapPbo = NULL;
    m_splatMapPbo = NULL;
    m_config = &Config::GetInstance();
    m_cursorPosition = Vector2i(0,0);
    m_cursorPositionWasUpdated = true;
    m_xzScale = 500.0f;
    m_yScale = 30.0f;
    m_resolution = 512;
    m_textureScale = 0.07f;
    m_chunks = 7;
    m_chunkSize = (m_resolution-1) / m_chunks;
    HEIGHT_MAP_SIZE = m_resolution * m_resolution * sizeof(unsigned short);
    SPLAT_MAP_SIZE = m_resolution * m_resolution * sizeof(SplatColor);

    // position the heightfield so that it is centered at the origin:
    m_offset = Vector3f(-m_xzScale/2.0f,0,-m_xzScale/2.0f);

    m_grassTexture = LoadTexture("img/grass.png");

    m_dirtTexture = LoadTexture("img/dirt.png");

    m_rockTexture = LoadTexture("img/rock.png");

    /*
      load the shader
    */


    string shaderName = "shader/height_map_render";


    {
	vector<string> defines;
	defines.push_back("SHADOW_MAPPING");

	m_shader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }

    m_depthShader = ShaderProgram::Load("shader/height_map_output_depth");

    {

	vector<string> defines;

	m_envShader =
	     ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }



    if(guiMode) {
	m_noise = new ValueNoise(2);
//	m_idShader = ShaderProgram::Load("shader/height_map_output_id");
	m_cursorShader = ShaderProgram::Load("shader/height_map_cursor");
    }

    CreateHeightmap(heightMapFilename, guiMode);

    CreateSplatMap(splatMapFilename, guiMode);

    CreateAoMap(aoMapFilename, guiMode);

    if(guiMode) {
	m_heightMapPbo = new PBO<unsigned short>(m_heightMap, m_heightData, HEIGHT_MAP_SIZE);
	m_splatMapPbo = new PBO<SplatColor>(m_splatMap, m_splatData, SPLAT_MAP_SIZE);

    }

    CreateAABBs();

    if(!guiMode){
	// if not in GUI, we do not need this array beyond this point.
	MY_DELETE(m_heightData);
    }
}

void HeightMap::CreateAoMap(const std::string& aoMapFilename, bool guiMode){

    m_aoData = new MultArray<float>(m_resolution, m_resolution, 1.0f);
    MultArray<float>& aoData = *m_aoData;


    if(aoMapFilename != "") {

	size_t unused;
	float* data = (float *)File::ReadArray(aoMapFilename, unused);

	for(int j = 0; j < m_resolution; ++j) {

	    for(int i = 0; i < m_resolution; ++i) {

		aoData(i,j) = *data;

		++data;
	    }
	}


    }

/*
    for(size_t x = 0; x < m_resolution; ++x) {

	for(size_t z = 0; z < m_resolution; ++z) {

	    float ao = 0.0f;

	    Vector3f originPos = ComputeHeightMapPos(x,z);
	    Vector3f originNormal = ComputeHeightMapNormal(x,z);

	    for(int i=1; i<63; i++){

		float s = i / 32.0f;
		float a = sqrt(s*300.0f);

		float b = sqrt(s * 200.0f);

		float spiralX = sin(a)*b;
		float spiralZ = cos(a)*b;

		int ix = int(x + spiralX);
		int iz = int(z + spiralZ);

		ix = Clamp(ix, 0, m_resolution-1);
		iz = Clamp(iz, 0, m_resolution-1);


		//  aoData(ix, iz) = 0.5f;

		Vector3f samplePos = ComputeHeightMapPos(ix,iz);

		Vector3f sampleDir = (samplePos - originPos).Normalize();

		float lambert = Clamp(Vector3f::Dot(originNormal, sampleDir), 0.0f, 1.0f);

		float distFactor = 1.7/sqrt((samplePos - originPos).Length());
		ao += distFactor*lambert;

	    }

	    aoData(x,z) = ao/32.0;



	}

    }
*/

    m_aoMap = new Texture2D(aoData.GetData(), m_resolution, m_resolution,

			    GL_R16F, // internal format
			    GL_RED, // format
			    GL_FLOAT);

    m_aoMap->Bind();
    m_aoMap->SetTextureClamping();
    m_aoMap->GenerateMipmap();
    m_aoMap->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_aoMap->SetMagFilter(GL_LINEAR);
    m_aoMap->Unbind();

    if(!guiMode){
	// if not in GUI, we do not need this array beyond this point.
	MY_DELETE(m_aoData);
    }

}

void HeightMap::SetCursorSize(int cursorSize) {
    m_cursorSize = cursorSize;

    CreateCursor();
}

HeightMap::HeightMap( bool guiMode) {
    Init("", "", "", guiMode);
}

HeightMap::HeightMap(const std::string& heightMapFilename, const std::string& splatMapFilename,
		         const std::string& aoMapFilename,bool guiMode ){

    Init(heightMapFilename, splatMapFilename, aoMapFilename, guiMode);

}

HeightMap::~HeightMap() {

    MY_DELETE(m_shader);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_grassTexture);
}


void HeightMap::CreateCursor() {

    vector<Vector2f> points;

    const float rad = m_cursorSize;

    for(int ix = -rad; ix <= +rad; ++ix) {

	for(int iz = -rad; iz <= +rad; ++iz) {

	    float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );
	    int idist = dist;

	    if( idist == m_cursorSize || idist == 0) {
		points.push_back(Vector2f((float)ix / (float)m_resolution, (float)iz / (float)m_resolution));
	    }
	}
    }

    m_cursorVertexBuffer->Bind();
    m_cursorVertexBuffer->SetBufferData(points);
    m_cursorVertexBuffer->Unbind();

    m_numCursorPoints = (unsigned short)points.size();

    if(m_tempData) {
	MY_DELETE(m_tempData);
    }

    m_tempData = new MultArray<unsigned short>(m_cursorSize*2+1, m_cursorSize*2+1, (unsigned short)0  );

}

void HeightMap::RenderSetup(ShaderProgram* shader) {
    shader->SetUniform("heightMap", 3);
    Texture::SetActiveTextureUnit(3);
    m_heightMap->Bind();




    shader->SetUniform("xzScale", m_xzScale);
    shader->SetUniform("yScale", m_yScale);
    shader->SetUniform("offset", m_offset);
    shader->SetUniform("resolution", (float)m_resolution);
    shader->SetUniform("textureScale", (float)m_textureScale);
    shader->SetUniform("chunks", (float)m_chunks );

}

void HeightMap::RenderUnsetup() {
    m_heightMap->Unbind();
}



void HeightMap::RenderEnvMapSetup(bool aoOnly) {

    m_envShader->Bind();


    m_envShader->SetUniform("aoOnly", aoOnly ? 1.0f : 0.0f);


    m_envShader->SetUniform("splatMap", 4);
    Texture::SetActiveTextureUnit(4);
    m_splatMap->Bind();

    m_envShader->SetUniform("aoMap", 5);
    Texture::SetActiveTextureUnit(5);
    m_aoMap->Bind();



    m_envShader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();

    m_envShader->SetUniform("dirt", 1);
    Texture::SetActiveTextureUnit(1);
    m_dirtTexture->Bind();

    m_envShader->SetUniform("rock", 2);
    Texture::SetActiveTextureUnit(2);
    m_rockTexture->Bind();


    RenderSetup(m_envShader);


    // setup vertex buffers.
    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_indexBuffer->Bind();

}

void HeightMap::RenderEnvMapUnsetup() {
    m_indexBuffer->Unbind();

    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();

    RenderUnsetup();



    m_grassTexture->Unbind();
    m_dirtTexture->Unbind();
    m_rockTexture->Unbind();

    m_splatMap->Unbind();
    m_aoMap->Unbind();

    m_envShader->Unbind();

}

void HeightMap::RenderEnvMap(const ICamera* camera, const Vector4f& lightPosition, int i) {

    m_envShader->SetPhongUniforms(Matrix4f::CreateTranslation(0,0,0), camera, lightPosition);

    MultArray<bool>& inEnvFrustum = *m_inEnvFrustums[i];

    for(int x = 0; x < m_chunks; ++x) {
	for(int z = 0; z < m_chunks; ++z) {

	    if(!inEnvFrustum(x,z)) {
		// not in frustum, dont draw.
		continue;
	    }

	    //	    LOG_I("draw chunk");

	    m_envShader->SetUniform("chunkPos", Vector2f(x,z) );

	    // DRAW.
	    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);
	}
    }
}


void HeightMap::Render(ShaderProgram* shader, bool shadows) {

    RenderSetup(shader);


    // setup vertex buffers.
    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_indexBuffer->Bind();

    MultArray<bool>& inCameraFrustum = *m_inCameraFrustum;
    MultArray<bool>& inLightFrustum = *m_inLightFrustum;;

    for(int x = 0; x < m_chunks; ++x) {
	for(int z = 0; z < m_chunks; ++z) {

	    if(shadows) {

		if(!inLightFrustum(x,z)) {
		    // not in frustum, dont draw.
		    continue;
		}

	    } else {

		if(!inCameraFrustum(x,z)) {
		    // not in frustum, dont draw.
		    continue;
		}

	    }

	    shader->SetUniform("chunkPos", Vector2f(x,z) );

	    // DRAW.
	    m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);
	}
    }

    // DRAW.
    // m_indexBuffer->DrawIndices(GL_TRIANGLES, (m_numTriangles)*3);

    // unsetup vertex buffer.

    m_indexBuffer->Unbind();

    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();


    RenderUnsetup();
}

void HeightMap::RenderHeightMap(
    const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap,
    bool aoOnly) {

    m_shader->Bind();

    m_shader->SetPhongUniforms(Matrix4f::CreateTranslation(0,0,0), camera, lightPosition, lightVp);

    m_shader->SetUniform("aoOnly", aoOnly ? 1.0f : 0.0f);


    m_shader->SetUniform("splatMap", 4);
    Texture::SetActiveTextureUnit(4);
    m_splatMap->Bind();

    m_shader->SetUniform("aoMap", 5);
    Texture::SetActiveTextureUnit(5);
    m_aoMap->Bind();



    m_shader->SetUniform("grass", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();

    m_shader->SetUniform("dirt", 1);
    Texture::SetActiveTextureUnit(1);
    m_dirtTexture->Bind();

    m_shader->SetUniform("rock", 2);
    Texture::SetActiveTextureUnit(2);
    m_rockTexture->Bind();

    m_shader->SetUniform("shadowMap", (int)shadowMap.GetTargetTextureUnit() );
    Texture::SetActiveTextureUnit(shadowMap.GetTargetTextureUnit());
    shadowMap.GetRenderTargetTexture().Bind();

    // set textures and stuff.

    Render(m_shader,false);

    m_grassTexture->Unbind();
    m_dirtTexture->Unbind();
    m_rockTexture->Unbind();

    m_splatMap->Unbind();
    m_aoMap->Unbind();

    shadowMap.GetRenderTargetTexture().Unbind();

    m_shader->Unbind();
}


void HeightMap::RenderCursor(const ICamera* camera) {




    m_cursorShader->Bind();

    m_cursorShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);


    m_cursorShader->SetUniform("cursorPos",
			       Vector2f((float)m_cursorPosition.x, (float)m_cursorPosition.y) );

    m_cursorShader->SetUniform("cameraPos",
			       camera->GetPosition() );


    // set point size.
    glPointSize(7.0);

    RenderSetup(m_cursorShader);

    m_cursorVertexBuffer->EnableVertexAttribInterleaved();
    m_cursorVertexBuffer->DrawVertices(GL_POINTS, m_numCursorPoints);
    m_cursorVertexBuffer->DisableVertexAttribInterleaved();

    RenderUnsetup();

    m_cursorShader->Unbind();
}


void HeightMap::Render(
    const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap,
    const bool aoOnly) {


    RenderHeightMap(camera, lightPosition, lightVp, shadowMap, aoOnly);

    if(m_config->IsGui()) {
	RenderCursor(camera);
    }



/*
    MultArray<AABB>& aabbs = *m_aabbs;
    MultArray<bool>& inCameraFrustum = *m_inCameraFrustum;

    for(int x = 0; x < m_chunks; ++x) {
	for(int z = 0; z < m_chunks; ++z) {

	    if(inCameraFrustum(x,z)) {

		AABB aabb = aabbs(x,z);

		Vector3f center = (aabb.min + aabb.max) * 0.5f;
		Vector3f radius = aabb.max - center;

		m_aabbWireframe->SetModelMatrix(
		    Matrix4f::CreateTranslation(center) *
		    Matrix4f::CreateScale(radius));

		m_aabbWireframe->Render(camera->GetVp());
	    }
	}
    }
*/

}

void HeightMap::RenderShadowMap(const Matrix4f& lightVp) {
    m_depthShader->Bind();

    m_depthShader->SetUniform("mvp", lightVp); // model matrix is identity, so we do not need to multiply by model.

    Render(m_depthShader,true);

    m_depthShader->Unbind();
}
/*
void HeightMap::RenderId(const ICamera* camera) {

    m_idShader->Bind();

    m_idShader->SetShaderUniforms(Matrix4f::CreateTranslation(0,0,0), camera);

    Render(m_idShader);

    m_idShader->Unbind();
}
*/
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


void HeightMap::LoadSplatMap(const std::string& splatMapFilename) {

    size_t unused;
    SplatColor* data = (SplatColor *)File::ReadArray(splatMapFilename, unused);

    MultArray<SplatColor>& splatData = *m_splatData;

    for(int j = 0; j < m_resolution; ++j) {

	for(int i = 0; i < m_resolution; ++i) {

	    splatData(i,j) = *data;

	    ++data;
	}
    }


    /*
      unsigned int width;
      unsigned int height;


      std::string* resourcePath = ResourceManager::GetInstance().SearchResource(splatMapFilename);
      if(!resourcePath) {
      PrintErrorExit();
      }

      std::vector<unsigned char> tempBuffer;
      lodepng::load_file(tempBuffer,
      *resourcePath);

      lodepng::State state;
      unsigned int unused1;
      unsigned int unused2;

      std::vector<unsigned char> buffer;
      unsigned error =
      lodepng::decode(buffer, unused1, unused2, tempBuffer, LCT_RGBA, 8);

      //lodepng::decode(buffer, width, height, state, buffer);

      if(error != 0){
      SetError("could not load splstmap png %s: %s", resourcePath->c_str(), lodepng_error_text(error));
      PrintErrorExit();
      }



      unsigned char* splatData = (unsigned char*)m_splatData->GetData();

      for(size_t i = 0; i < buffer.size(); ++i) {
      *splatData = buffer[i];
      ++splatData;
      }

      //  vector<unsigned char>& imageData = textureInfo->imageData;
      */
}


void HeightMap::LoadHeightmap(const std::string& heightMapFilename) {

    size_t unused;
    unsigned short* data = (unsigned short *)File::ReadArray(heightMapFilename, unused);

    MultArray<unsigned short>& heightData = *m_heightData;

    for(int j = 0; j < m_resolution; ++j) {

	for(int i = 0; i < m_resolution; ++i) {



	    heightData(i,j) = *data;

	    ++data;
	}
    }

    /*
      std::string* resourcePath = ResourceManager::GetInstance().SearchResource(heightMapFilename);
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
      unsigned error = lodepng::decode(buffer, unused1, unused2, tempBuffer, LCT_GREY, 16);

      if(error != 0){
      SetError("could not loa heightmapd png %s: %s", resourcePath->c_str(), lodepng_error_text(error));
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
    */
}

void HeightMap::CreateSplatMap(const std::string& splatMapFilename, bool guiMode) {

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

    if(splatMapFilename != "") {
	LoadSplatMap(splatMapFilename);
    }

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

    if(!guiMode){
	// if not in GUI, we do not need this array beyond this point.
	MY_DELETE(m_splatData);
    }
}

void HeightMap::CreateHeightmap(const std::string& heightMapFilename, bool guiMode) {

    size_t width = m_resolution;
    size_t depth = m_resolution;

    m_heightData = new MultArray<unsigned short>(width, depth, (unsigned short)0);

    MultArray<unsigned short>& heightData = *m_heightData;

    if(heightMapFilename == "") { // if no heightmap to load.

	for(size_t i = 0; i < width; ++i) {

	    for(size_t j = 0; j < depth; ++j) {

		heightData(j,i) = MID_HEIGHT;
	    }
	}
    } else {
	LoadHeightmap(heightMapFilename);
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


    /*
      Create a chunk mesh:
    */

    // how many verties wide a chunk is.
    int chunkVertices = (m_chunkSize+1);

    MultArray<Cell> *m_chunk = new MultArray<Cell>(chunkVertices+1,chunkVertices+1);
    MultArray<Cell> &chunk = *m_chunk;

    unsigned int xpos = 0;
    unsigned int zpos = 0;

    for(size_t i = 0; i < (chunkVertices+1)*(chunkVertices+1); ++i) {

	Cell& c = chunk(xpos, zpos);

	// x,z will vary in range [0,1]
	float x = (float)(xpos) / (float)chunkVertices;
	float z = (float)(zpos) / (float)chunkVertices;

	/*
	  This is very wasteful! We y is always 0, so we do not need a Vector3f
  Vector2f is enough.
	 */
	c.position =
	    Vector2f(
		x,
		z
		);

	++xpos;
	if(xpos != 0 && ( (xpos) % (chunkVertices+1) == 0)) {
	    xpos = 0;
	    ++zpos;
	}
    }

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{2} // pos.
	);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(chunk);
    m_vertexBuffer->Unbind();

    m_cursorVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{2}, // pos
	GL_DYNAMIC_DRAW
	);

    GLuint baseIndex = 0;
    UintVector indices;

    m_numTriangles = 0;

    for(size_t x = 0; x < (m_chunkSize+1); ++x) {
	for(size_t z = 0; z < (m_chunkSize+1); ++z) {

	    indices.push_back(baseIndex+chunkVertices +1);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+0);

	    indices.push_back(baseIndex+chunkVertices+1+1);
	    indices.push_back(baseIndex+1);
	    indices.push_back(baseIndex+chunkVertices+1);

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

float HeightMap::GetHeightAt(float, float)const {


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
	hit = hit * (1.0f / m_xzScale);
	hit = hit * (float)m_resolution;

	int xHit = (int)hit.x;
	int zHit = (int)hit.z;

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



void HeightMap::DistortTerrain(const float delta, const float strength, float noiseScale) {

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& heightData = *m_heightData;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    float rad = m_cursorSize;
    float fade_rad = m_cursorSize-5;

    if(total > 0.05) {

	total = 0;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int ax = cx+ix;
		int az = cz+iz;

		if(
		    ax < 0 || ax >= m_resolution ||
		    az < 0 || az >= m_resolution) {
		    continue; // out of range.
		}


		// distance from center of hill.
		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		// if within the radius of the hill(this ensures that the hill is round)
		if(dist <= rad) {

		    // y is some value  [-1,1]
		    float y = m_noise->Turbulence(
			12,
			noiseScale* Vector2f(cx+ix,cz+iz),
			2.0f,
			0.5f);

		    if(dist > fade_rad) {
			float x2 = (dist - fade_rad  ) /  (rad-fade_rad);
			y *= x2;
		    }

		    // maximum height of the hill
		    float maxHeight =y * (float)(MAX_HEIGHT-MID_HEIGHT);

		    /*
		      Note that cx and cz describe the center position of the hill.
		    */

		    // if we hold down the mouse for 30 frames, the hill will reach its maximum height
		    float increment = maxHeight * strength; //

		    if(heightData(ax,az) + increment > MAX_HEIGHT) {
			heightData(ax,az) = MAX_HEIGHT;
		    } else if(heightData(ax,az) + increment < MIN_HEIGHT) {
			heightData(ax,az) = MIN_HEIGHT;
		    } else {
			heightData(ax,az) += increment;
		    }

		}


	    } // end for
	} // end for

	m_heightMapPbo->RequestUpdate();
    }
}

void HeightMap::SmoothTerrain(const float delta, const int smoothRadius) {

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& heightData = *m_heightData;
    MultArray<unsigned short>& tempData = *m_tempData;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    float rad = m_cursorSize;

    if(total > 0.1) {

	total = 0;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int ax = cx+ix;
		int az = cz+iz;


		if(!InBounds(ax,az)) {
		    continue; // out of range.
		}

		// distance from center of hill.
		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		// if within the radius of the hill(this ensures that the hill is round)
		if(dist <= rad) {


		    int sum = 0;
		    int num = 0;

		    const int FILTER_RAD = smoothRadius;

		    for(int mx = -FILTER_RAD; mx <= +FILTER_RAD; ++mx) {
			for(int mz = -FILTER_RAD; mz <= +FILTER_RAD; ++mz) {

			    if(InBounds(ax+mx,az+mz)) {

				sum += heightData(ax+mx,az+mz);
				++num;
			    }
			}
		    }

		    unsigned short average = sum / num;

		    tempData(ix+m_cursorSize, iz+m_cursorSize) = average;

		} else {
		    tempData(ix+m_cursorSize, iz+m_cursorSize) =
			heightData(ax,az);
		    // then value should be unchanged.
		}

	    } // end for
	} // end for


	// now transfer over the averaged values:

	for(int ix = 0; ix <= 2*rad; ++ix) {

	    for(int iz = 0; iz <= 2*rad; ++iz) {

//		LOG_I("lal3");
		if(InBounds(cx+ix-m_cursorSize, cz+iz-m_cursorSize)) {


		    unsigned short avg = tempData(ix,iz);
		    heightData(cx+ix-m_cursorSize, cz+iz-m_cursorSize) = avg;
		}
//		LOG_I("lal4");

	    }

	}

	m_heightMapPbo->RequestUpdate();

    }

}


void HeightMap::ModifyTerrain(const float delta, const float strength) {

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& heightData = *m_heightData;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    float fade_rad = m_cursorSize-5;
    float rad = m_cursorSize;

    if(total > 0.05) {

	total = 0;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int ax = cx+ix;
		int az = cz+iz;


		if(!InBounds(ax,az)) {
		    continue; // out of range.
		}

		// distance from center of hill.
		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		// if within the radius of the hill(this ensures that the hill is round)
		if(dist <= rad) {

		    // the farther we get from the center, the less we increase the height.
		    // this effectively results in a radial gradient being added to the
		    // heightmap.
		    // Note that we use "x*x" instead of "x", because this results in a
		    // more round and natural-looking hill
		    float y;
		    float x = dist / rad;
		    y = (1.0 - x*x);

		    if(dist > fade_rad) {
			float x2 = (dist - fade_rad  ) /  (rad-fade_rad);
			y *=  (1.0f- x2*x2  );
		    }

		    // maximum height of the hill
		    float maxHeight =y * (float)(MAX_HEIGHT-MID_HEIGHT);

		    /*
		      Note that cx and cz describe the center position of the hill.
		    */

		    // if we hold down the mouse for 30 frames, the hill will reach its maximum height
		    float increment = maxHeight * strength; //

		    if(heightData(ax,az) + increment > MAX_HEIGHT) {
			heightData(ax,az) = MAX_HEIGHT;
		    } else if(heightData(ax,az) + increment < MIN_HEIGHT) {
			heightData(ax,az) = MIN_HEIGHT;
		    } else {
			heightData(ax,az) += increment;
		    }

		}


	    } // end for
	} // end for

	m_heightMapPbo->RequestUpdate();
    }
}



void HeightMap::LevelTerrain(const float delta, const float strength) {

    static float total = 0;

    total += delta;

    MultArray<unsigned short>& heightData = *m_heightData;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    float rad = m_cursorSize;

    if(total > 0.05) {

	total = 0;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int ax = cx+ix;
		int az = cz+iz;

		if(!InBounds(ax,az)) {
		    continue; // out of range.
		}

		// distance from center of hill.
		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		// if within the radius of the hill(this ensures that the hill is round)
		if(dist <= rad) {

		    unsigned short INC =strength * (float)(MAX_HEIGHT-MID_HEIGHT);

		    unsigned short h = heightData(ax,az);

		    unsigned short newH;

		    if(h == MID_HEIGHT) {
			newH = MID_HEIGHT;
		    } else if(abs(h - MID_HEIGHT) < INC ) {
			newH = MID_HEIGHT;
		    } else if(h < MID_HEIGHT) {
			newH = h + INC;
		    } else { // h > MID_HEIGHT
			newH = h - INC;
		    }

		    heightData(ax,az) = newH;



		}


	    } // end for
	} // end for


	m_heightMapPbo->RequestUpdate();
    }
}


void HeightMap::DrawTexture(const float delta, int drawTextureType) {

    MultArray<SplatColor>& splatData = *m_splatData;

    float rad = m_cursorSize;

    float fadeRad = (m_cursorSize-5 );

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    if(m_cursorPositionWasUpdated) {

	// we need to move cursor again to draw again.
	m_cursorPositionWasUpdated = false;

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int px = cx+ix;
		int pz = cz+iz;

		if(!InBounds(px,pz)) {
		    continue; // out of range.
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


			unsigned char* textures =  (unsigned char *)&splatData(
			    px ,
			    pz
			    );

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

	m_splatMapPbo->RequestUpdate();

    } else {

    }
}


void HeightMap::UpdateGui(const float delta, ICamera* camera,
		       const float framebufferWidth,
		       const float framebufferHeight) {

    if(m_config->IsGui() && camera) {
	UpdateCursor(camera,framebufferWidth, framebufferHeight);
    }


    if(m_heightMapPbo)
	m_heightMapPbo->Update();

    if(m_splatMapPbo)
	m_splatMapPbo->Update();


}

void HeightMap::SaveHeightMap(const std::string& filename) {

    unsigned short* data = m_heightMap->GetPixels<unsigned short>(m_resolution * m_resolution, GL_RED, GL_UNSIGNED_SHORT);

    File::WriteArray(filename, data, m_resolution * m_resolution*2);

//    m_heightMap->Write16ToFile("height.png");
}

void HeightMap::SaveSplatMap(const std::string& filename) {

    MultArray<SplatColor>& splatData = *m_splatData;

    File::WriteArray(filename, splatData.GetData(), m_resolution * m_resolution*4);
}

void HeightMap::SaveAoMap(const std::string& filename) {

    MultArray<float>& aoData = *m_aoData;

    File::WriteArray(filename, aoData.GetData(),
		     m_resolution * m_resolution*4 // every float is 4 bytes.
	);
}

void HeightMap::AddToPhysicsWorld(PhysicsWorld* physicsWorld) {

    Vector3f* LandscapeVtx = new Vector3f[m_resolution*m_resolution];
    Vector3f* LandscapeVtxBeg = LandscapeVtx;

    unsigned short* rawHeightMap = m_heightMap->GetPixels<unsigned short>(
	m_resolution* m_resolution * 1, GL_RED, GL_UNSIGNED_SHORT
	);
    unsigned int xpos = 0;
    unsigned int zpos = 0;

    for(size_t i = 0; i < m_resolution*m_resolution; ++i) {

	Vector3f pos(
	    (float)xpos / (float)m_resolution,
	    0,
	    (float)zpos / (float)m_resolution

	    );


	unsigned short us = *rawHeightMap;

	signed short ss =
	    us >= MID_HEIGHT ?
	    us - MID_HEIGHT :
	    -(MID_HEIGHT -us);
	float h =  ss / (float)MID_HEIGHT;


	// scale us to [-1,1], then use formula.

	float SCALE = 1.0f;
	Vector3f v =
	    m_offset + Vector3f(
		pos.x * m_xzScale * SCALE,
	       	h*m_yScale,
		pos.z * m_xzScale * SCALE);


	*LandscapeVtx = v;
	++LandscapeVtx;

	++xpos;
	if(xpos != 0 && ( xpos % (m_resolution) == 0)) {

	    xpos = 0;
	    ++zpos;
	}

	++rawHeightMap;
    }

    unsigned int numTriangles = 0;
    GLuint baseIndex = 0;
    //UintVector indices;
    unsigned int* indices = new unsigned int[(m_resolution-1)*(m_resolution-1)*6];
    unsigned int* indicesBeg = indices;

    numTriangles = 0;


    for(size_t x = 0; x < (m_resolution-1); ++x) {
	for(size_t z = 0; z < (m_resolution-1); ++z) {


	    *indices = baseIndex+m_resolution;
	    indices++;


	    *indices = baseIndex+1;
	    indices++;

	    *indices = baseIndex+0;
	    indices++;



	    *indices = baseIndex+m_resolution+1;
	    indices++;


	    *indices = baseIndex+1;
	    indices++;

	    *indices = baseIndex+m_resolution;
	    indices++;


	    numTriangles+=2;

	    baseIndex += 1;
	}
	baseIndex += 1;
    }


    btTransform trans;
    trans.setIdentity();


    btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
    btIndexedMesh part;

    part.m_vertexBase = (const unsigned char*)LandscapeVtxBeg;
    part.m_vertexStride = sizeof(btScalar) * 3;
    part.m_numVertices = m_resolution * m_resolution;
    part.m_triangleIndexBase =   (const unsigned char*)indicesBeg;
    part.m_triangleIndexStride = sizeof(unsigned int) * 3;

    part.m_numTriangles = numTriangles;
    part.m_indexType = PHY_INTEGER;

    meshInterface->addIndexedMesh(part,PHY_INTEGER);

    bool	useQuantizedAabbCompression = true;
    btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(meshInterface,true,true);


    btTriangleInfoMap* triangleInfoMap = NULL;
    triangleInfoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo(trimeshShape, triangleInfoMap);



    btVector3 localInertia(0,0,0);

    btDefaultMotionState* motionState = new btDefaultMotionState(trans);



    btVector3 inertia(0,0,0);

    btRigidBody::btRigidBodyConstructionInfo ci(0.0f, motionState, trimeshShape, inertia);

    ci.m_friction=2.0f;

    btRigidBody* body = new btRigidBody(ci);




    body->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    physicsWorld->AddRigidBody(body,  COL_STATIC, staticCollidesWith);

}

bool HeightMap::InBounds(int x, int z) {

    return
	x >= 0 && x < m_resolution &&
		      z >= 0 && z < m_resolution;
}

//fix this;

Vector3f HeightMap::GetChunkCornerPos(int chunkX, int chunkZ, float y) {

    Vector2f globalPos(chunkX / (float)m_chunks, chunkZ / (float)m_chunks);

    return m_offset + Vector3f(globalPos.x * m_xzScale, y,globalPos.y * m_xzScale);
}

void HeightMap::CreateAABBs() {

    m_inCameraFrustum = new MultArray<bool>(m_chunks, m_chunks, true);
    m_inLightFrustum = new MultArray<bool>(m_chunks, m_chunks, true);

    for(int i = 0; i < 6; ++i) {
	m_inEnvFrustums[i] = new MultArray<bool>(m_chunks, m_chunks, true);
    }

    m_aabbs = new MultArray<AABB>(m_chunks, m_chunks);
    MultArray<AABB>& aabbs = *m_aabbs;

    for(int x = 0; x < m_chunks; ++x) {
	for(int z = 0; z < m_chunks; ++z) {

	    AABB aabb(
		GetChunkCornerPos(x,z, -m_yScale),
		GetChunkCornerPos(x+1, z+1, +m_yScale)
		);
	    aabbs(x,z) = aabb;
	}
    }
}

void HeightMap::Update(const ViewFrustum& cameraFrustum, const ViewFrustum& lightFrustum,
    ViewFrustum** envLightFrustums) {

    MultArray<AABB>& aabbs = *m_aabbs;
    MultArray<bool>& inCameraFrustum = *m_inCameraFrustum;
    MultArray<bool>& inLightFrustum = *m_inLightFrustum;



    for(int x = 0; x < m_chunks; ++x) {
	for(int z = 0; z < m_chunks; ++z) {

	    AABB aabb = aabbs(x,z);

	    inCameraFrustum(x,z) = cameraFrustum.IsAABBInFrustum(aabb);
	    inLightFrustum(x,z) = lightFrustum.IsAABBInFrustum(aabb);

	    for(int i = 0; i < 6; ++i) {

		MultArray<bool>& inEnvFrustums = *m_inEnvFrustums[i];
		inEnvFrustums(x,z) = envLightFrustums[i]->IsAABBInFrustum(aabb);

	    }


	}
    }



/*
    for(int i=0; i < 6; ++i) {
	LOG_I("count: %d, %d", i, count[i]);
    }
    LOG_I("STOP");*/
}



AABB HeightMap::GetAABB()const {
    AABB aabb;

    aabb.max = m_offset + Vector3f(
	1.0f * m_xzScale,
	1.0f*m_yScale,
	1.0f * m_xzScale);

    aabb.min = aabb.max * -1.0f;

    return aabb;
}

Vector3f HeightMap::ComputeHeightMapPos(int x, int z) {

    // clamp:
    if(x < 0)
	x = 0;
    if(x >= m_resolution)
	x = m_resolution-1;

    if(z < 0)
	z = 0;
    if(z >= m_resolution)
	z = m_resolution-1;

    return  Vector3f(
	((float)x / m_resolution) * m_xzScale,
	ComputeHeightMapHeight(x,z)*m_yScale,
         ((float)z / m_resolution) * m_xzScale );
}

float HeightMap::ComputeHeightMapHeight(int x, int z) {

    x = Clamp(x, 0, m_resolution-1);

    z = Clamp(z, 0, m_resolution-1);

    MultArray<unsigned short>& heightData = *m_heightData;
    return (float)(heightData(x,z)-MID_HEIGHT) / MID_HEIGHT;
}


Vector3f HeightMap::ComputeHeightMapNormal(int x, int z) {
    int eps = 1;

    //eps on x axis.
    Vector3f va =
	Vector3f(2*eps, ComputeHeightMapHeight(x+eps, z) - ComputeHeightMapHeight(x-eps,z), 0 );

    Vector3f vb = Vector3f(0, ComputeHeightMapHeight(x,z+eps) - ComputeHeightMapHeight(x,z-eps), 2*eps );

    // http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
    Vector3f n = (Vector3f::Cross(vb.Normalize(), va.Normalize() )).Normalize();

    return n;
}


void HeightMap::BakeAo(int samples, int waveLength, int amplitude, float distAttenuation) {

    MultArray<unsigned short>& heightData = *m_heightData;
    MultArray<float>& aoData = *m_aoData;

    for(size_t x = 0; x < m_resolution; ++x) {

	for(size_t z = 0; z < m_resolution; ++z) {

	    float ao = 0.0f;

	    Vector3f originPos = ComputeHeightMapPos(x,z);
	    Vector3f originNormal = ComputeHeightMapNormal(x,z);

	    for(int i=1; i<=samples; i++){

		float s = i / (float)samples;
		float a = sqrt(s* (float)waveLength );

		float b = sqrt(s * (float)amplitude );

		float spiralX = sin(a)*b;
		float spiralZ = cos(a)*b;

		int ix = int(x + spiralX);
		int iz = int(z + spiralZ);

		ix = Clamp(ix, 0, m_resolution-1);
		iz = Clamp(iz, 0, m_resolution-1);


		//  aoData(ix, iz) = 0.5f;

		Vector3f samplePos = ComputeHeightMapPos(ix,iz);

		Vector3f sampleDir = (samplePos - originPos).Normalize();

		float lambert = Clamp(Vector3f::Dot(originNormal, sampleDir), 0.0f, 1.0f);

		float distFactor = distAttenuation/sqrt((samplePos - originPos).Length());
		ao += distFactor*lambert;

	    }

	    aoData(x,z) = ao/(float)samples;
	}

    }

    m_aoMap->Bind();
    m_aoMap->UpdateTexture(aoData.GetData() );
    m_aoMap->Unbind();

}

void HeightMap::ErodeTerrain() {

    MultArray<unsigned short>& heightData = *m_heightData;

    int cx = m_cursorPosition.x;
    int cz = m_cursorPosition.y;

    float rad = m_cursorSize;

    constexpr int T = 1000;

    for(int i = 0; i < 100; ++i) {

	for(int ix = -rad; ix <= +rad; ++ix) {

	    for(int iz = -rad; iz <= +rad; ++iz) {

		int ax = cx+ix;
		int az = cz+iz;

		if(!InBounds(ax,az)) {
		    continue; // out of range.
		}

		// distance from center of hill.
		float dist = sqrt( (float)ix * (float)ix + (float)iz * (float)iz  );

		// if within the radius of the hill(this ensures that the hill is round)
		if(dist <= rad) {

		    // find dmax and dtotal
		    double dmax = 0;
		    double dtotal = 0;
		    double dis[9];

		    //	LOG_I("ax, az = %d, %d",  ax, az );

		    int i = 0;
		    for(int nx = -1; nx <= 1; ++nx) {
			for(int nz = -1; nz <= 1; ++nz) {
			    /*
			    if(nx == 0 && nz == 0)
				continue;

			    */
			    //	LOG_I("%d, %d",  heightData(ax,az), heightData(ax+nx,az+nz) );


			    dis[i] =
				heightData(ax,az) - heightData(ax+nx,az+nz);

			    //	LOG_I("dif: %d",  dis[i] );

			    if(dis[i] > T) {
				dtotal += dis[i];

				if(dmax < dis[i]) {
				    dmax = dis[i];
				}

			    }


			    ++i;
			}
		    }

		    i = 0;
		    bool flag = false;
		    for(int nx = -1; nx <= 1; ++nx) {
			for(int nz = -1; nz <= 1; ++nz) {

			    if(dis[i] > T) {
				//   LOG_I("dis: %d", dis[i]);
				flag = true;

				heightData(ax+nx,az+nz) =
				    heightData(ax+nx,az+nz) + (unsigned short)(0.3 * (dmax - T) * (  dis[i] / dtotal ));

			    }

			    ++i;

			}
		    }

//		    heightData(ax+0,az+0) -= dmax - T;

		    /*if(flag)

		      exit(1);
		    */



		    /*
		      unsigned short INC =strength * (float)(MAX_HEIGHT-MID_HEIGHT);

		      unsigned short h = heightData(ax,az);

		      unsigned short newH;


		      if(h == MID_HEIGHT) {
		      newH = MID_HEIGHT;
		      } else if(abs(h - MID_HEIGHT) < INC ) {
		      newH = MID_HEIGHT;
		      } else if(h < MID_HEIGHT) {
		      newH = h + INC;
		      } else { // h > MID_HEIGHT
		      newH = h - INC;
		      }

		      heightData(ax,az) = newH;
		    */


		}


	    } // end for
	} // end for

    }


    m_heightMapPbo->RequestUpdate();
}
