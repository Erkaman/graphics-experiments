
#pragma once

#include <string>
#include "math/color.hpp"
#include "math/vector4f.hpp"
#include "math/vector2f.hpp"

#include "ewa/mult_array.hpp"
#include "ewa/aabb.hpp"
#include "math/vector2i.hpp"

#include "ewa/gl/gl_common.hpp"
#include "ewa/gl/texture2d.hpp"

#include "gui_enum.hpp"

class VBO;
class ShaderProgram;
class ICamera;
class Texture;
class PerlinSeed;
class Texture;
class ShaderProgra;
class Config;
class ICamera;
class PhysicsWorld;
class ValueNoise;
class DepthFBO;
class Matrix4f;
class ViewFrustum;
class Cube;
class ViewFrustum;

template<typename T>
class PBO {
private:
    int m_updateCount; // how many more updates are necesssary.

    Texture2D* m_texture;
    MultArray<T> *m_textureData;
    const int TEXTURE_SIZE;

    unsigned int m_pboIds[2];


public:
    PBO(Texture2D* texture, MultArray<T> *textureData, const int textureSize):
	TEXTURE_SIZE(textureSize){
	m_texture = texture;
	m_textureData = textureData;

	RequestUpdate(); // need some initial updates to properly setup the PBOs

        GL_C(glGenBuffers(2, m_pboIds));
        GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[0]));
        GL_C(glBufferData(GL_PIXEL_UNPACK_BUFFER,TEXTURE_SIZE,0, GL_STREAM_DRAW));
        GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[1]));
        GL_C(glBufferData(GL_PIXEL_UNPACK_BUFFER,TEXTURE_SIZE,0, GL_STREAM_DRAW));
        GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
    }

    ~PBO() {
        glDeleteBuffers(2, m_pboIds);
    }

    void RequestUpdate() {
	m_updateCount = 2;
    }

    void Update() {

	static int index = 0;
	int nextIndex = 0;

	if(m_updateCount > 0) {

	    index = (index + 1) % 2;
	    nextIndex = (index + 1) % 2;

	    m_texture->Bind();


	    GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[index] ));

	    m_texture->UpdateTexture(0);


	    GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

	    m_texture->Unbind();

	    // bind PBO to update texture source
	    GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[nextIndex] ));

	    GL_C(glBufferData(GL_PIXEL_UNPACK_BUFFER, TEXTURE_SIZE, 0, GL_STREAM_DRAW));

	    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
						    GL_WRITE_ONLY);

	    if(ptr) {
		memcpy(ptr, m_textureData->GetData(), TEXTURE_SIZE );

		GL_C(glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER)); // release the mapped buffer
	    }

	    GL_C(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

	    --m_updateCount;
	}
    }


};

// the data associated with every triangle in the heightmap mesh.
struct Cell {
    Vector2f position; // specifies vertex x and z!
    //   float id;
//    Vector2f texCoord;
};

struct SplatColor{
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

enum HeightMapRenderMode {

    HEIGHT_MAP_RENDER_MODE_ENV_MAP0,
    HEIGHT_MAP_RENDER_MODE_ENV_MAP1,
    HEIGHT_MAP_RENDER_MODE_ENV_MAP2,
    HEIGHT_MAP_RENDER_MODE_ENV_MAP3,
    HEIGHT_MAP_RENDER_MODE_ENV_MAP4,
    HEIGHT_MAP_RENDER_MODE_ENV_MAP5,

    HEIGHT_MAP_RENDER_MODE_NORMAL,
    HEIGHT_MAP_RENDER_MODE_SHADOWS,
    HEIGHT_MAP_RENDER_MODE_REFLECTION,
    HEIGHT_MAP_RENDER_MODE_REFRACTION,
    HEIGHT_MAP_RENDER_MODE_COUNT,



};



class HeightMap {


private:

    struct Chunk {
	VBO* m_vertexBuffer;
	VBO* m_indexBuffer;
	unsigned int m_numTriangles;
    };

    Chunk* CreateChunk(float scaling);

    int m_guiMode;



    unsigned int m_numTriangles;
    bool m_isWireframe;

    Chunk* m_chunkVersions[HEIGHT_MAP_RENDER_MODE_COUNT];

    VBO* m_cursorVertexBuffer;

    unsigned short m_numCursorPoints;

    ShaderProgram* m_shader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.
//    ShaderProgram* m_idShader; //outputs only the id. Used for triangle picking in the height map.
    ShaderProgram* m_cursorShader;
    ShaderProgram* m_envShader;
    ShaderProgram* m_refractionShader;
    ShaderProgram* m_reflectionShader;
    ShaderProgram* m_cubeShader;

    Texture* m_grassTexture;
    Texture* m_dirtTexture;
    Texture* m_rockTexture;
    Texture* m_asphaltTexture;

    Texture2D* m_heightMap;
    Texture2D* m_splatMap;
    Texture2D* m_aoMap;


    MultArray<unsigned short>* m_heightData;
    MultArray<SplatColor>* m_splatData;

    MultArray<float>* m_aoData;

    MultArray<AABB>* m_aabbs;// the AABBs of the chunks.
    Cube* m_aabbWireframe;

    /*
    // information of whether the chunks are in the camera:
    MultArray<bool>* m_inCameraFrustum;
    MultArray<bool>* m_inLightFrustum;
    MultArray<bool>* m_inReflectionFrustum;
    */

//    MultArray<bool>* m_inEnvFrustums[6];

    std::vector<Vector2i>* m_inCameraFrustum;
    std::vector<Vector2i>* m_inLightFrustum;
    std::vector<Vector2i>* m_inReflectionFrustum;

    std::vector<Vector2i>* m_inEnvFrustums[6];


    // used to store temp data in SmoothTerrain()
    MultArray<unsigned short>* m_tempData;

    Config* m_config;

    Vector2i m_cursorPosition;
    bool m_cursorPositionWasUpdated;

    Vector3f m_offset;
    float m_xzScale;
    float m_yScale;
    int m_resolution;
    int m_chunks; // how many chunks to subdivide the mesh into, on one axis.
    int m_chunkSize; // how many quads per chunk width.

    float m_textureScale;
    int HEIGHT_MAP_SIZE;
    int SPLAT_MAP_SIZE;

    int m_cursorSize;

    ValueNoise* m_noise;

    PBO<unsigned short>* m_heightMapPbo;
    PBO<SplatColor>* m_splatMapPbo;



    VBO* m_cubeIndexBuffer;
    VBO* m_cubePositionBuffer;
    GLushort m_cubeNumIndices;

    std::vector<Vector2i> m_controlPoints;

    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    void CreateHeightmap(const std::string& heightMapFilename, bool guiMode);
    void CreateCursor();
    void CreateCube();
    void CreateSplatMap(const std::string& splatMapFilename, bool guiMode);
    void CreateAABBs();

    void LoadHeightmap(const std::string& heightMapFilename);
    void LoadSplatMap(const std::string& splatMapFilename);

    void RenderHeightMap(
	const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap,
	const bool aoOnly);
    void RenderCursor(const ICamera* camera);
    void RenderCubeCursor(const ICamera* camera);

    void Render(ShaderProgram* shader, HeightMapRenderMode renderMode);

    void RenderSetup(ShaderProgram* shader);
    void RenderUnsetup();

    void UpdateCursor(ICamera* camera,
		      const float framebufferWidth,
		      const float framebufferHeight);

    void Init(
	const std::string& heightMapFilename,
	const std::string& splatMapFilename,
	const std::string& aoMapFilename,
	bool guiMode );

    bool InBounds(int x, int z);

    Vector3f GetChunkCornerPos(int chunkX, int chunkZ, float y);

public:

    HeightMap(
	const std::string& heightMapFilename,
	const std::string& splatMapFilename,
	const std::string& aoMapFilename,
	bool guiMode );
    HeightMap(bool guiMode );


    ~HeightMap();

    void Render(const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap, const bool aoOnly);

    // render, but instead of outputting colors for every triangle, we output the id of the frontmost triangles.
//    void RenderId(const ICamera* camera);

    void RenderShadowMap(const Matrix4f& lightVp);


    void RenderEnvMap(const ICamera* camera, const Vector4f& lightPosition, int i, bool aoOnly);


    void RenderRefraction(
	const ICamera* camera, const Vector4f& lightPosition, bool aoOnly);

    void RenderReflection(
	const ICamera* camera, const Vector4f& lightPosition, bool aoOnly);

    void SetWireframe(const bool wireframe);


    // need to be updated every frame if there is a gui.
    void UpdateGui(const float delta, ICamera* camera,
		const float framebufferWidth,
		const float framebufferHeight
	);

    void Update(const ViewFrustum& cameraFrustum, const ViewFrustum& lightFrustum,
		ViewFrustum** envLightFrustums,
		const ViewFrustum& reflectionFrustum);



    void ModifyTerrain(const float delta, const float strength);
    void DistortTerrain(const float delta, const float strength, float noiseScale);
    void SmoothTerrain(const float delta, const int smoothRadius);
    void LevelTerrain(const float delta, const float strength);
    void ErodeTerrain();

    void DrawTexture(const float delta, int drawTextureType);


    void SaveHeightMap(const std::string& filename);
    void SaveSplatMap(const std::string& filename);
    void SaveAoMap(const std::string& filename);

    void AddToPhysicsWorld(PhysicsWorld* physicsWorld);

    void SetCursorSize(int cursorSize);

    AABB GetAABB()const;

    void CreateAoMap(const std::string& aoMapFilename, bool guiMode);
    void BakeAo(int samples=32, int waveLength=300, int amplitude=200, float distAttenuation = 1.7);

    Vector3f ComputeHeightMapPos(int x, int z);
    float ComputeHeightMapHeight(int x, int z);
    Vector3f ComputeHeightMapNormal(int x, int z);

    void SetGuiMode(int guiMode);

    void AddControlPoint();

    void BuildRoad();
    void DeleteCP();

    std::vector<std::string> GetDefaultDefines()const;

    int GetResolution()const {
	return m_resolution;
    }

    Texture2D* GetHeightMap() {
	    return m_heightMap;
    }

};
