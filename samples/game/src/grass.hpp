#pragma once

#include "ewa/gl/gl_common.hpp"

#include "ewa/math/vector2i.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/mult_array.hpp"
#include "ewa/aabb.hpp"


#include "ewa/random.hpp"

#include <map>

class VBO;
class ShaderProgram;
class ICamera;
class Texture;
class Vector2f;
class Vector3f;
class Vector4f;
class HeightMap;
class Vector2i;
class Texture2D;
class ViewFrustum;


struct GrassInfo {
public:
    Vector2f pos;
    float angle;
    float size;

    int baseIndex;
};

class Grass{

private:

    // turbulent grass tile
    struct GrassTile {
    public:

	Vector3f m_dir;
	Vector3f m_v;

	float m_xDelta;
	float m_zDelta;


	GrassTile(Random& rng);

	Vector3f Update(const float delta);

    };


    int m_currentId;

    Random m_rng;


    std::map<int,GrassInfo> m_grass;

    struct GrassChunk {
    public:
	unsigned int m_grassNumTriangles;

	VBO* m_grassVertexBuffer;
	VBO* m_grassIndexBuffer;

	FloatVector m_grassVertices;
	std::vector<GLuint> m_grassIndices;
    };


    ShaderProgram* m_deferredShader;
    ShaderProgram* m_reflectionShader;
    ShaderProgram* m_outputIdShader;

    float m_heightMapResolution;
    float m_chunkSize;

    Texture* m_grassTexture;

    float m_time;

    HeightMap* m_heightMap;

    Vector2f m_cameraPosition;
    Vector3f m_cameraDir;
    float m_xzScale;

    MultArray<AABB>* m_aabbs;

    Texture2D* m_meanWindTexture;
    MultArray<Vector3f>* m_meanWindTextureBuffer;

//new MultArray<unsigned short>


    // turbulent wind.
    Texture2D* m_turbWindTexture;
    MultArray<Vector3f>* m_turbWindTextureBuffer;

    MultArray<GrassTile*>* m_turbWindField;

    MultArray<GrassChunk*>* m_chunks;

    std::vector<Vector2i>* m_inCameraFrustum;
    std::vector<Vector2i>* m_inReflectionFrustum;
    std::vector<Vector2i>* m_inEnvFrustums[6];

    void GenerateGrassVertices(const Vector2f position, const float angle, const float width, const float height, int id);


    void MakeGrass(const Vector2f position, const float angle, const float width, const float height, int id);

    void Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader,
	const std::vector<Vector2i>& inFrustum);

    void Rebuild();

    void Init(HeightMap* heightMap);



    void UpdateWind(const float delta);


    void CreateAABBs(const float yScale, const Vector3f& offset, float xzScale);

    Vector3f GetChunkCornerPos(int chunkX, int chunkZ, float y, const Vector3f& offset, float xzScale);


public:

    Grass(const std::string& filename, HeightMap* heightMap);
    Grass(HeightMap* heightMap);

    ~Grass();

    void DrawDeferred(const ICamera* camera, const Vector4f& lightPosition);
    void DrawReflection(const ICamera* camera, const Vector4f& lightPosition);
    void DrawEnvMap(const ICamera* camera, const Vector4f& lightPosition, int i);

    void RenderIdAll(const ICamera* camera);

    void Update(const float delta, const Vector2f& cameraPosition, const Vector3f& cameraDir,
		const ViewFrustum& cameraFrustum, const ViewFrustum& lightFrustum,
		ViewFrustum** envLightFrustums, const ViewFrustum& reflectionFrustum);

    void AddGrass(const Vector2i& position, int grassClusterSize);
    void RemoveGrass(int id);

    void SaveGrass(const std::string& filename);

    GLuint GetBaseIndex(FloatVector& grassVertices);

};
