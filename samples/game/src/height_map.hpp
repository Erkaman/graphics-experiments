// TODO:we can remove the y component as an easy optimization!
// TODO: since we have such high resolution, we might as well stretch the map some, to save triangles.
// TODO: also, get rid of id from the mesh! we do not need it!




























#pragma once

#include <string>
#include "math/color.hpp"
#include "math/vector4f.hpp"
#include "math/vector2f.hpp"

#include "ewa/mult_array.hpp"
#include "math/vector2i.hpp"

#include "ewa/gl/gl_common.hpp"
#include "ewa/gl/texture2d.hpp"

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
    Vector3f position;
    float id;
    Vector2f texCoord;
};

struct SplatColor{
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

};

class HeightMap {


private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    VBO* m_cursorVertexBuffer;
    unsigned short m_numCursorPoints;

    ShaderProgram* m_shader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.
    ShaderProgram* m_idShader; //outputs only the id. Used for triangle picking in the height map.
    ShaderProgram* m_cursorShader;

    Texture* m_grassTexture;
    Texture* m_dirtTexture;
    Texture* m_rockTexture;

    Texture2D* m_heightMap;
    Texture2D* m_splatMap;

    MultArray<Cell> *m_map;
    MultArray<unsigned short>* m_heightData;
    MultArray<SplatColor>* m_splatData;

    // used to store temp data in SmoothTerrain()
    MultArray<unsigned short>* m_tempData;

    Config* m_config;

    Vector2i m_cursorPosition;
    bool m_cursorPositionWasUpdated;

    Vector3f m_offset;
    float m_xzScale;
    float m_yScale;
    int m_resolution;
    float m_textureScale;
    int HEIGHT_MAP_SIZE;
    int SPLAT_MAP_SIZE;

    int m_cursorSize;

    ValueNoise* m_noise;

    PBO<unsigned short>* m_heightMapPbo;
    PBO<SplatColor>* m_splatMapPbo;

    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    void CreateHeightmap(const std::string& heightMapFilename, bool guiMode);
    void CreateCursor();
    void CreateSplatMap(const std::string& splatMapFilename, bool guiMode);

    void LoadHeightmap(const std::string& heightMapFilename);
    void LoadSplatMap(const std::string& splatMapFilename);

    void RenderHeightMap(
	const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);
    void RenderCursor(const ICamera* camera);


    void Render(ShaderProgram* shader);

    void RenderSetup(ShaderProgram* shader);
    void RenderUnsetup();

    void UpdateCursor(ICamera* camera,
		      const float framebufferWidth,
		      const float framebufferHeight);

    void Init(const std::string& heightMapFilename, const std::string& splatMapFilename, bool guiMode );

    bool InBounds(int x, int z);

public:

    HeightMap(const std::string& heightMapFilename, const std::string& splatMapFilename, bool guiMode );
    HeightMap(bool guiMode );


    ~HeightMap();

    void Render(const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap);

    // render, but instead of outputting colors for every triangle, we output the id of the frontmost triangles.
    void RenderId(const ICamera* camera);

    void RenderShadowMap(const Matrix4f& lightVp);

    void SetWireframe(const bool wireframe);

    float GetHeightAt(float x, float z)const;

    void Update(const float delta, ICamera* camera,
		const float framebufferWidth,
		const float framebufferHeight);

    void ModifyTerrain(const float delta, const float strength);
    void DistortTerrain(const float delta, const float strength, float noiseScale);
    void SmoothTerrain(const float delta, const int smoothRadius);
    void LevelTerrain(const float delta, const float strength);

    void DrawTexture(const float delta, int drawTextureType);


    void SaveHeightMap(const std::string& filename);
    void SaveSplatMap(const std::string& filename);

    void AddToPhysicsWorld(PhysicsWorld* physicsWorld);

    void SetCursorSize(int cursorSize);
};
