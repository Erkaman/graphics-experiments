#include "grass.hpp"
#include "biased_random.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/math/math_common.hpp"

#include "ewa/value_noise_seed.hpp"
#include "ewa/icamera.hpp"
#include "height_map.hpp"

#include "ewa/util.hpp"
#include "ewa/random.hpp"
#include "ewa/mult_array.hpp"
#include "ewa/common.hpp"
#include "resource_manager.hpp"

#include "ewa/buffered_file_reader.hpp"
#include "ewa/string_util.hpp"

#include "math/math_common.hpp"

#include "ewa/view_frustum.hpp"
#include "whirl_pattern_creator.hpp"



using std::string;
using std::vector;
using std::to_string;

constexpr int TILE_GRID_COUNT = 64;
constexpr int CHUNK_COUNT = 20;

Vector2f AngleToVector(const float angle) {
    const float radians = ToRadians(angle);
    return Vector2f(
	cos(radians),
	sin(radians)
	);
}

Grass::~Grass() {
}


void Grass::Init(HeightMap* heightMap  ) {
    m_heightMapResolution = heightMap->GetResolution();

    m_currentId = 0;

    /*
      Create the skydome.
    */

    m_time = 0;

    CreateAABBs( heightMap->GetYScale(), heightMap->GetOffset(),heightMap->GetXzScale()  );
    m_xzScale = heightMap->GetXzScale() ;

    m_chunks = new MultArray<GrassChunk*>(CHUNK_COUNT, CHUNK_COUNT);
    m_chunkSize = m_heightMapResolution / (float)CHUNK_COUNT;
    MultArray<GrassChunk*>& chunks = *m_chunks;


    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {
	    GrassChunk* chunk = new GrassChunk();

	    chunk->m_grassNumTriangles = 0;
	    chunk->m_grassVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,3,3, 3} // pos, texcoord, normal, slot0
		);

	    chunk->m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_INT);
	    chunk->m_grassIndexBuffer->SetUsage(GL_DYNAMIC_DRAW);

	    chunks(x,z) = chunk;

	}
    }








    vector<string> defaultDefines = m_heightMap->GetDefaultDefines();
    string shaderName = "shader/grass";

    {
	vector<string> defines(defaultDefines);
//	defines.push_back("SHADOW_MAPPING");


	defines.push_back("DEFERRED");

	m_deferredShader =
	    ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }



    {
	vector<string> defines(defaultDefines);

	m_reflectionShader =
	    ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }


    {
	vector<string> defines(defaultDefines);
	defines.push_back("OUTPUT_ID");

	m_outputIdShader =
	    ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);
    }


    m_grassTexture = Texture2D::Load("img/grass_billboard.png");

    if(!m_grassTexture) {
	PrintErrorExit();
    }

    m_grassTexture->Bind();
    m_grassTexture->SetTextureClamping();
    m_grassTexture->GenerateMipmap();


   m_grassTexture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_grassTexture->SetMagFilter(GL_LINEAR);


    m_grassTexture->Unbind();




    m_meanWindTexture = new Texture2D(nullptr, TILE_GRID_COUNT, TILE_GRID_COUNT,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_meanWindTexture->Bind();
    m_meanWindTexture->SetTextureClamping();
    m_meanWindTexture->SetMinFilter(GL_NEAREST);
    m_meanWindTexture->SetMagFilter(GL_NEAREST);
    m_meanWindTexture->Unbind();

    m_meanWindTextureBuffer = new MultArray<Vector3f>(TILE_GRID_COUNT,TILE_GRID_COUNT);
    MultArray<Vector3f>& meanWindTextureBuffer = *m_meanWindTextureBuffer;

    m_blowWindTextureBuffer = new MultArray<Vector3f>(TILE_GRID_COUNT,TILE_GRID_COUNT);


    for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	for(int z = 0; z < TILE_GRID_COUNT; ++z) {

	    Vector3f w;
/*
  int rem = (i   ) % 2 + (i%2 == 0 ? 0 : 1);

  if(rem == 0) {
  w = Vector3f(1,1,1);
  } else if(rem == 1) {
  w = Vector3f(0,0,0);
  }*/
	    w =Vector3f(
		Vector3f(m_rng.RandomFloat(-2,+2), 0, m_rng.RandomFloat(-2,+2)).Normalize()

		);
	    w = Vector3f(0,0,0);

	    meanWindTextureBuffer(x,z) = w;
	}
    }

    m_meanWindTexture->Bind();
    m_meanWindTexture->UpdateTexture(m_meanWindTextureBuffer->GetData() );
    m_meanWindTexture->Unbind();




    m_turbWindTexture = new Texture2D(nullptr, TILE_GRID_COUNT, TILE_GRID_COUNT,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_turbWindTexture->Bind();
    m_turbWindTexture->SetTextureClamping();
    m_turbWindTexture->SetMinFilter(GL_NEAREST);
    m_turbWindTexture->SetMagFilter(GL_NEAREST);
    m_turbWindTexture->Unbind();

    m_turbWindTextureBuffer = new MultArray<Vector3f>(TILE_GRID_COUNT,TILE_GRID_COUNT);

    m_turbWindField = new MultArray<GrassTile*>(TILE_GRID_COUNT,TILE_GRID_COUNT);
    MultArray<GrassTile*>& turbWindField = *m_turbWindField;


    for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	for(int z = 0; z < TILE_GRID_COUNT; ++z) {

	    turbWindField(x,z) = new GrassTile(m_rng);
	}
    }


}

Grass::Grass(HeightMap* heightMap): m_rng(12), m_heightMap(heightMap) {
    Init(heightMap );
}


Grass::Grass(const std::string& filename, HeightMap* heightMap): m_rng(12), m_heightMap(heightMap) {

    Init(heightMap);

    BufferedFileReader* reader = BufferedFileReader::Load(  filename);
    if(!reader) {
	PrintErrorExit();
    }

    string firstLine = reader->ReadLine();

    size_t numGrass = stoi(StringUtil::SplitString(firstLine, " ")[1]);

    LOG_I("numGrass: %d", numGrass);

    for(size_t i = 0; i < numGrass; ++i) {

	// read shit.

	vector<string> tokens = StringUtil::SplitString(reader->ReadLine(), " ");

	GrassInfo grass;

	grass.pos =  Vector2f( stof(tokens[1] ), stof(tokens[2])  );

	grass.angle = stof(tokens[3]);
	grass.size = stof(tokens[4]);

	m_grass[m_currentId++] = grass;

    }

    Rebuild();

}


void Grass::Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader,
    const std::vector<Vector2i>& inFrustum) {

    SetCullFace(false);

    shader->Bind();
    shader->SetUniform("time", m_time);

    const Matrix4f model =  Matrix4f::CreateIdentity();

    shader->SetPhongUniforms(model, camera, lightPosition, Matrix4f::CreateIdentity() );

    shader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);
    m_grassTexture->Bind();


    shader->SetUniform("heightMap", 1);
    Texture::SetActiveTextureUnit(1);
    m_heightMap->GetHeightMap()->Bind();

    shader->SetUniform("meanWindTex", 2);
    Texture::SetActiveTextureUnit(2);
    m_meanWindTexture->Bind();


    shader->SetUniform("turbWindTex", 3);
    Texture::SetActiveTextureUnit(3);
    m_turbWindTexture->Bind();



    shader->SetUniform("cameraPos", m_cameraPosition );
    shader->SetUniform("cameraDir", m_cameraDir );


    MultArray<GrassChunk*>& chunks = *m_chunks;


    /*
    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {
*/
    for(Vector2i chunkPos : inFrustum) {

	int x = chunkPos.x;
	int z = chunkPos.y;

	    GrassChunk* chunk = chunks(x,z);

	    // if( (x+z) % 2 == 1 )

	    VBO::DrawIndices(*chunk->m_grassVertexBuffer, *chunk->m_grassIndexBuffer, GL_TRIANGLES, (chunk->m_grassNumTriangles)*3);


    }



    m_grassTexture->Unbind();
    m_meanWindTexture->Unbind();
    m_turbWindTexture->Unbind();

    shader->Unbind();

    SetCullFace(true);

}

void Grass::DrawDeferred(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_deferredShader,*m_inCameraFrustum);
}

void Grass::DrawReflection(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_reflectionShader,*m_inReflectionFrustum);
}

void Grass::DrawEnvMap(const ICamera* camera, const Vector4f& lightPosition, int i) {
    Draw(camera, lightPosition, m_reflectionShader,* m_inEnvFrustums[i]);
}



void Grass::Update(const float delta, const Vector2f& cameraPosition, const Vector3f& cameraDir,
		   const ViewFrustum& cameraFrustum, const ViewFrustum& lightFrustum,
		   ViewFrustum** envLightFrustums, const ViewFrustum& reflectionFrustum) {

	
    MultArray<AABB>& aabbs = *m_aabbs;


    m_inCameraFrustum->clear();
    m_inReflectionFrustum->clear();

    for(int i = 0; i < 6; ++i) {
	m_inEnvFrustums[i]->clear();
    }





    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {

	    AABB aabb = aabbs(x,z);

	    Vector2i v(x,z);


	    if(cameraFrustum.IsAABBInFrustum(aabb)) {
		m_inCameraFrustum->push_back(v);
	    }


	    if(reflectionFrustum.IsAABBInFrustum(aabb)) {
		m_inReflectionFrustum->push_back(v);
	    }

	    for(int i = 0; i < 6; ++i) {
		if(envLightFrustums[i]->IsAABBInFrustum(aabb)) {
		    m_inEnvFrustums[i]->push_back(v);
		}
	    }
	}
    }

	
    if(m_doWind) {

	float tileSize = m_heightMapResolution / TILE_GRID_COUNT;


	MultArray<Vector3f>& meanWindTextureBuffer = *m_meanWindTextureBuffer;


	float windForceLerp = (1.0 - m_windRadius / 30.0f);

	float windForce = windForceLerp * sqrt(windForceLerp) * 0.35;

	for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	    for(int z = 0; z < TILE_GRID_COUNT; ++z) {


		Vector3f tileCenter(
		    x * tileSize + tileSize / 2.0f,
		    0,
		    z * tileSize + tileSize / 2.0f
		);

		Vector3f diff = (tileCenter - m_windCenter).Normalize();

		float radius = (tileCenter - m_windCenter).Length();

		if(  (fabs(radius - m_windRadius) <  10.0f) && windForce > 0.0f
		    ) {

			meanWindTextureBuffer(x,z) += diff * windForce;

		} else {

		    meanWindTextureBuffer(x,z) *= 0.97f;



		}


	    }
	}


	m_windRadius += 1.0f;

	
	m_meanWindTexture->Bind();
	m_meanWindTexture->UpdateTexture(m_meanWindTextureBuffer->GetData() );
	m_meanWindTexture->Unbind();
	



    }







   UpdateWind(delta);

    m_time += delta;





    m_cameraPosition = cameraPosition;
    m_cameraDir = cameraDir;


}

void Grass::GenerateGrassVertices(const Vector2f position, const float angle, const float width, const float height, int id) {

    int ix = int(position.x / m_chunkSize);
    int iz = int(position.y / m_chunkSize);
    MultArray<GrassChunk*>& chunks = *m_chunks;

    GrassChunk* chunk = chunks(ix,iz);

    FloatVector& grassVertices = chunk->m_grassVertices;
    std::vector<GLuint>& grassIndices = chunk->m_grassIndices;

    GLuint baseIndex = GetBaseIndex(grassVertices);

    Vector2f dir = AngleToVector(angle);
    Vector3f normal(0,1,0);

    Vector3f centerPosition(position.x ,position.y, id);

//    LOG_I("ADD CENTER: %s", string(centerPosition).c_str()  );

    dir.Normalize();

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    float h2 = height / 2.0f;

    (Vector3f(-X, +h2, -Z)).Add(grassVertices);
    Vector3f(0.0f,0.0f,h2).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, +h2, +Z)).Add(grassVertices);
    Vector3f(1.0f,0.0f,h2).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(-X, -h2, -Z)).Add(grassVertices);
    Vector3f(0.0f,1.0f,h2).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, -h2, +Z)).Add(grassVertices);
    Vector3f(1.0f,1.0f,h2).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);


    grassIndices.push_back(baseIndex+0);
    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+2);

    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+3);
    grassIndices.push_back(baseIndex+2);


    chunk->m_grassNumTriangles += 2;
}

void Grass::MakeGrass(const Vector2f position, const float angle, const float width, const float height, int id) {
    GenerateGrassVertices(position, 0+angle, width,height, id);
    GenerateGrassVertices(position, 60+angle, width,height, id);
    GenerateGrassVertices(position, 120+angle, width,height, id);

}


void Grass::Rebuild() {
/*
    FloatVector grassVertices;
    vector<GLuint> grassIndices;
    m_grassNumTriangles = 0;
*/
    MultArray<GrassChunk*>& chunks = *m_chunks;

    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {
	    GrassChunk* chunk = chunks(x,z);

	    chunk->m_grassVertices.clear();
	    chunk->m_grassIndices.clear();
	    chunk->m_grassNumTriangles = 0;
	}
    }




      constexpr float SPREAD = 10.0f;

    for(auto& pair : m_grass) {

	GrassInfo& grass = pair.second;
	int id = pair.first;

	//grass
	// generates indices will be in range [baseIndex, baseIndex + 6 * 3]
//    grass.baseIndex = GetBaseIndex(grassVertices);

      MakeGrass(grass.pos, grass.angle, grass.size, grass.size,id);


    }


    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {
	    GrassChunk* chunk = chunks(x,z);

	    chunk->m_grassVertexBuffer->Bind();
	    chunk->m_grassVertexBuffer->SetBufferData(chunk->m_grassVertices);
	    chunk->m_grassVertexBuffer->Unbind();

	    chunk->m_grassIndexBuffer->Bind();
	    chunk->m_grassIndexBuffer->SetBufferData(chunk->m_grassIndices);
	    chunk->m_grassIndexBuffer->Unbind();
	}
    }


}

void Grass::AddGrass(const Vector2i& position, int grassClusterSize) {

    GL_C(glFlush() ) ;
    GL_C(glFinish() ) ;


    int numGrass = (grassClusterSize-1) * m_rng.RandomFloat(+4,+6);
    int dist = (grassClusterSize-1) * 3.5;

    if(numGrass <= 0)
	numGrass = 1;
    if(dist <= 0.01) {
	dist = 0;
    }


    for(int i = 0; i < numGrass; ++i ) {

	GrassInfo grass;

	grass.pos =  Vector2f(
	    (float)position.x + m_rng.RandomFloat(-dist,+dist) + m_rng.RandomFloat(-1,+1),
	    (float)position.y + m_rng.RandomFloat(-dist,+dist) + m_rng.RandomFloat(-1,+1));
	grass.angle = m_rng.RandomFloat(-90,+90);
	grass.size =m_rng.RandomFloat(2.0f,2.7f);

	m_grass[m_currentId++] = grass;

    }


    //  LOG_I("grass count: %d", m_grass.size() );


    Rebuild();

    GL_C(glFlush() ) ;
    GL_C(glFinish() ) ;

}


void Grass::RemoveGrass(int id) {
    m_grass.erase(id);
    Rebuild();
}


void Grass::RenderIdAll(const ICamera* camera) {
    Draw(camera, Vector4f(0), m_outputIdShader, *m_inCameraFrustum);
}

void Grass::SaveGrass(const std::string& filename) {


    File* outFile = File::Load(
	filename,
	    FileModeWriting);



    outFile->WriteLine("numGrass " + to_string(m_grass.size()) );


    for(auto& pair : m_grass) {

	GrassInfo grass = pair.second;


	string line = "grass ";

	line +=
	    to_string(grass.pos.x) + " " +
	    to_string(grass.pos.y) + " " +
	    to_string(grass.angle) + " " +
	    to_string(grass.size);


	outFile->WriteLine(line);
    }
}

GLuint Grass::GetBaseIndex(FloatVector& grassVertices) {
     return  (GLuint)grassVertices.size() / (3+3+3+3);
}


void Grass::UpdateWind(const float delta) {

    MultArray<Vector3f>& turbWindTextureBuffer = *m_turbWindTextureBuffer;
    MultArray<GrassTile*>& turbWindField = *m_turbWindField;

    for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	for(int z = 0; z < TILE_GRID_COUNT; ++z) {
	    turbWindField(x,z)->Update(delta);
	}
    }

    // weights
    int ws[3][3] =
	{
	    { 1, 2, 1 },
	    { 2, 4, 2 },
	    { 1, 2, 1 },
	};

    // filter radius.
    int rad = 1;

    int offset = 1;
    float denominator = 0;

    for(int ix = -rad; ix <= +rad; ++ix) {
	for(int iz = -rad; iz <= +rad; ++iz) {
	    denominator += ws[ix+offset][iz+offset];
	}
    }
    denominator = 1.0 / denominator;

//    LOG_I("den om: %f", denominator  );

    for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	for(int z = 0; z < TILE_GRID_COUNT; ++z) {

	    Vector3f numerator = 0.0f;

	    for(int ix = -rad; ix <= +rad; ++ix) {
		for(int iz = -rad; iz <= +rad; ++iz) {

		    int ax = x+ix;
		    int az = z+iz;

		    ax = Clamp(ax, 0, TILE_GRID_COUNT-1);
		    az = Clamp(az, 0, TILE_GRID_COUNT-1);

		    numerator += turbWindField(ax,az)->m_v * ws[ix+offset][iz+offset];

		}
	    }

//	    float

	    turbWindTextureBuffer(x,z) = numerator * denominator;


	}
    }


	
    m_turbWindTexture->Bind();
    m_turbWindTexture->UpdateTexture(m_turbWindTextureBuffer->GetData() );
    m_turbWindTexture->Unbind();
	

}


Grass::GrassTile::GrassTile(Random& rng):

    m_dir( Vector3f(rng.RandomFloat(-2,+2), 0, rng.RandomFloat(-2,+2))  )
{
    m_xDelta = rng.RandomFloat(-100,+1000);
    m_zDelta = rng.RandomFloat(-1000,+10000);
}

void Grass::GrassTile::Update(const float delta) {

    m_xDelta += delta;
    m_zDelta += delta;

    m_v = Vector3f(
	m_dir.x * sin(m_xDelta),
	0,
	m_dir.z * cos(m_xDelta)

	);



//    return Vector3f(0,0,0);
}

Vector3f Grass::GetChunkCornerPos(int chunkX, int chunkZ, float y, const Vector3f& offset, float xzScale) {

    Vector2f globalPos(chunkX / (float)CHUNK_COUNT, chunkZ / (float)CHUNK_COUNT);

    return offset + Vector3f(globalPos.x * xzScale, y,globalPos.y * xzScale);
}


void Grass::CreateAABBs(const float yScale, const Vector3f& offset, float xzScale) {

    m_inCameraFrustum = new std::vector<Vector2i>;
    m_inCameraFrustum->reserve(CHUNK_COUNT * CHUNK_COUNT);

    m_inReflectionFrustum = new std::vector<Vector2i>;
    m_inReflectionFrustum->reserve(CHUNK_COUNT * CHUNK_COUNT);

    for(int i = 0; i < 6; ++i) {
	m_inEnvFrustums[i] =new std::vector<Vector2i>;
	m_inEnvFrustums[i]->reserve(CHUNK_COUNT * CHUNK_COUNT);
    }

    m_aabbs = new MultArray<AABB>(CHUNK_COUNT, CHUNK_COUNT);
    MultArray<AABB>& aabbs = *m_aabbs;

    for(int x = 0; x < CHUNK_COUNT; ++x) {
	for(int z = 0; z < CHUNK_COUNT; ++z) {

	    AABB aabb(
		GetChunkCornerPos(x,z, -yScale, offset, xzScale),
		GetChunkCornerPos(x+1, z+1, +yScale, offset, xzScale)
		);
	    aabbs(x,z) = aabb;

//	    LOG_I("aabb: %s, %s", string(aabb.min).c_str(), string(aabb.max).c_str() );
	}
    }
}


void Grass::BlowWind() {
    LOG_I("BLOW");

    float AREA = 10.0f;

    m_windCenter = Vector3f(
	235.650131 + m_rng.RandomFloat(-AREA,+AREA),
	0,
	297.401459 + m_rng.RandomFloat(-AREA,+AREA));
    m_windRadius = 0.5f;

/*
    MultArray<Vector3f>& blowWindTextureBuffer = *m_blowWindTextureBuffer;

    float tileSize = m_heightMapResolution / TILE_GRID_COUNT;

    for(int x = 0; x < TILE_GRID_COUNT; ++x) {
	for(int z = 0; z < TILE_GRID_COUNT; ++z) {

	    // tile center.
	    Vector2f tileCenter(
		x * tileSize + tileSize / 2.0f,
		z * tileSize + tileSize / 2.0f
		);

//	    LOG_I("tile center: %s", string(tileCenter).c_str() );

	    Vector2f diff = (tileCenter - windCenter);
	    float dist = diff.Length();
	    Vector2f w =   20.0f * diff.Normalize() * ( 1.0f / (1.0f + 1.8f*dist ) ) ;
//	    w =

	    blowWindTextureBuffer(x,z) =Vector3f(w.x, 0, w.y);
//	    meanWindTextureBuffer(x,z) =
	}
    }
*/

    /*
    m_meanWindTexture->Bind();
    m_meanWindTexture->UpdateTexture(m_meanWindTextureBuffer->GetData() );
    m_meanWindTexture->Unbind();
    */

/*    windLerp = 0;
    doWind = true;
    windInc = 2.8f;*/

    m_doWind = true;
}
