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


using std::string;
using std::vector;
using std::to_string;

constexpr int GRID_COUNT = 64;


Vector2f AngleToVector(const float angle) {
    const float radians = ToRadians(angle);
    return Vector2f(
	cos(radians),
	sin(radians)
	);
}

Grass::~Grass() {
    MY_DELETE(m_grassVertexBuffer);
    MY_DELETE(m_grassIndexBuffer);



    MY_DELETE(m_grassTexture);
}


void Grass::Init() {

    m_currentId = 0;

    /*
      Create the skydome.
    */

    m_time = 0;

    m_grassNumTriangles = 0;

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

    m_grassVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3,2,3, 3} // pos, texcoord, normal, slot0
	);
    m_grassIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_INT);
    m_grassIndexBuffer->SetUsage(GL_DYNAMIC_DRAW);



    m_meanWindTexture = new Texture2D(nullptr, GRID_COUNT, GRID_COUNT,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_meanWindTexture->Bind();
    m_meanWindTexture->SetTextureClamping();
    m_meanWindTexture->SetMinFilter(GL_NEAREST);
    m_meanWindTexture->SetMagFilter(GL_NEAREST);
    m_meanWindTexture->Unbind();

    m_meanWindTextureBuffer = new Vector3f[GRID_COUNT*GRID_COUNT];

    for(int i = 0; i < GRID_COUNT*GRID_COUNT; ++i) {

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

	m_meanWindTextureBuffer[i] = w;
    }

    m_meanWindTexture->Bind();
    m_meanWindTexture->UpdateTexture(&m_meanWindTextureBuffer[0] );
    m_meanWindTexture->Unbind();




    m_turbWindTexture = new Texture2D(nullptr, GRID_COUNT, GRID_COUNT,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_turbWindTexture->Bind();
    m_turbWindTexture->SetTextureClamping();
    m_turbWindTexture->SetMinFilter(GL_NEAREST);
    m_turbWindTexture->SetMagFilter(GL_NEAREST);
    m_turbWindTexture->Unbind();

    m_turbWindTextureBuffer = new Vector3f[GRID_COUNT*GRID_COUNT];

    m_turbWindField = new Grass::GrassTile*[GRID_COUNT*GRID_COUNT];


    for(int i = 0; i < GRID_COUNT*GRID_COUNT; ++i) {

	m_turbWindField[i] = new GrassTile(m_rng);
    }


}

Grass::Grass(HeightMap* heightMap): m_rng(12), m_heightMap(heightMap) {
    Init();
}


Grass::Grass(const std::string& filename, HeightMap* heightMap): m_rng(12), m_heightMap(heightMap) {

    Init();


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


void Grass::Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader) {

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

//    LOG_I("camera pos: %s", string(m_cameraPosition).c_str()  );


    VBO::DrawIndices(*m_grassVertexBuffer, *m_grassIndexBuffer, GL_TRIANGLES, (m_grassNumTriangles)*3);

    m_grassTexture->Unbind();
    m_meanWindTexture->Unbind();
    m_turbWindTexture->Unbind();

    shader->Unbind();

    SetCullFace(true);

}

void Grass::DrawDeferred(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_deferredShader);
}

void Grass::DrawReflection(const ICamera* camera, const Vector4f& lightPosition) {
    Draw(camera, lightPosition, m_reflectionShader);
}

void Grass::Update(const float delta, const Vector2f& cameraPosition, const Vector3f& cameraDir) {

    UpdateWind(delta);

    m_time += delta;

//    LOG_I("cam pos: %s", string(cameraPosition).c_str() );

    m_cameraPosition = cameraPosition;
    m_cameraDir = cameraDir;

    vector<GrassInfo> grassVector;

    for(auto& pair : m_grass) {

	GrassInfo grass = pair.second;
	grassVector.push_back(grass);
    }



    const Vector2f cp = m_cameraPosition;
    std::sort(grassVector.begin(), grassVector.end(), [cp](const GrassInfo& a, const GrassInfo& b) {


	    float bDist = (b.pos.x - cp.x)*(b.pos.x - cp.x) + (b.pos.y - cp.y)*(b.pos.y - cp.y);
	    float aDist = (a.pos.x - cp.x)*(a.pos.x - cp.x) + (a.pos.y - cp.y)*(a.pos.y - cp.y);

	    return bDist > aDist;
    });


    vector<GLuint> grassIndices;

/*
    LOG_I("BEG");
    LOG_I("cp: %s", string(cp).c_str() );
*/

    for(int i = 0; i < grassVector.size(); ++i) {
	GrassInfo& grass = grassVector[i];

	int baseIndex = grass.baseIndex;

	for(int i = 0; i < 3; ++i) {

	    grassIndices.push_back(baseIndex+0);
	    grassIndices.push_back(baseIndex+1);
	    grassIndices.push_back(baseIndex+2);

	    grassIndices.push_back(baseIndex+1);
	    grassIndices.push_back(baseIndex+3);
	    grassIndices.push_back(baseIndex+2);

	    baseIndex += 4;


    	}


    }
    // LOG_I("DONE");

    m_grassIndexBuffer->Bind();
    m_grassIndexBuffer->SetBufferData(grassIndices);
    m_grassIndexBuffer->Unbind();

}

void Grass::GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices,
				  vector<GLuint>& grassIndices, const float width, const float height, int id) {
    GLuint baseIndex = GetBaseIndex(grassVertices);

    Vector2f dir = AngleToVector(angle);
    Vector3f normal(0,1,0);

    Vector3f centerPosition(position.x ,position.y, id);

//    LOG_I("ADD CENTER: %s", string(centerPosition).c_str()  );

    dir.Normalize();

    const float X = dir.x * width / 2.0f;
    const float Z = dir.y * width / 2.0f;

    (Vector3f(-X, height, -Z)).Add(grassVertices);
    Vector2f(0.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, height, +Z)).Add(grassVertices);
    Vector2f(1.0f,0.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(-X, 0, -Z)).Add(grassVertices);
    Vector2f(0.0f,1.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    (Vector3f(+X, 0, +Z)).Add(grassVertices);
    Vector2f(1.0f,1.0f).Add(grassVertices);
    normal.Add(grassVertices);
    centerPosition.Add(grassVertices);

    /*
    grassIndices.push_back(baseIndex+0);
    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+2);

    grassIndices.push_back(baseIndex+1);
    grassIndices.push_back(baseIndex+3);
    grassIndices.push_back(baseIndex+2);
    */
    m_grassNumTriangles += 2;
}

void Grass::MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, vector<GLuint>& grassIndices, FloatVector& billboardVertices, vector<GLuint>& billboardIndices, const float width, const float height, int id) {
    GenerateGrassVertices(position, 0+angle,grassVertices, grassIndices, width,height, id);
    GenerateGrassVertices(position, 60+angle,grassVertices, grassIndices, width,height, id);
    GenerateGrassVertices(position, 120+angle,grassVertices, grassIndices, width,height, id);

//    GenerateBillboardVertices(position - m_position,billboardVertices, billboardIndices, width,height);
}


void Grass::Rebuild() {

    FloatVector grassVertices;
    vector<GLuint> grassIndices;
    m_grassNumTriangles = 0;

      FloatVector billboardVertices;
      vector<GLuint> billboardIndices;

      constexpr float SPREAD = 10.0f;

    for(auto& pair : m_grass) {

	GrassInfo& grass = pair.second;
	int id = pair.first;

	//grass
	// generates indices will be in range [baseIndex, baseIndex + 6 * 3]
    grass.baseIndex = GetBaseIndex(grassVertices);

      MakeGrass(grass.pos, grass.angle , grassVertices, grassIndices, billboardVertices,
		billboardIndices, grass.size, grass.size,id);


    }


    m_grassVertexBuffer->Bind();
    m_grassVertexBuffer->SetBufferData(grassVertices);
    m_grassVertexBuffer->Unbind();




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
    Draw(camera, Vector4f(0), m_outputIdShader );
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
     return  (GLuint)grassVertices.size() / (3+2+3+3);
}


void Grass::UpdateWind(const float delta) {
    for(int i = 0; i < GRID_COUNT*GRID_COUNT; ++i) {


	m_turbWindTextureBuffer[i] = m_turbWindField[i]->Update(delta);
    }

    m_turbWindTexture->Bind();
    m_turbWindTexture->UpdateTexture(&m_turbWindTextureBuffer[0] );
    m_turbWindTexture->Unbind();

}


Grass::GrassTile::GrassTile(Random& rng):
    m_minAngle(rng.RandomFloat(-2,-0.5)  ),
    m_maxAngle(rng.RandomFloat(0.5,+2.0)  ),
    m_angle(0),
    m_origVector( Vector3f(rng.RandomFloat(-2,+2), 0, rng.RandomFloat(-2,+2)).Normalize()  ),
    m_vectorLength(rng.RandomFloat(0.5,+2.5)),
    m_rotationVel(rng.RandomFloat(-0.5, +0.5)){

}

Vector3f Grass::GrassTile::Update(const float delta) {

    m_angle += m_rotationVel * delta;

    if(m_angle < m_minAngle) {
	m_angle = m_minAngle;
	m_rotationVel *= -1.0f;
    } else if(m_angle > m_maxAngle) {
	m_angle = m_maxAngle;
	m_rotationVel *= -1.0f;
    }


    float a = m_angle;
    float x = m_origVector.x;
    float y = m_origVector.z;

    return m_vectorLength * Vector3f(
	cos(a) * x - sin(a) * y,
	0,
	sin(a) * x + cos(a) * y
	);



//    return Vector3f(0,0,0);
}
