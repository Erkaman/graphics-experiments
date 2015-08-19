#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"

struct GrassBlade {
    Vector3f vertexPosition;
    float grassHeight;
    float grassWidth;
    Vector3f windDirection;
    int lod;
    int grassTexture;
};

struct GrassGroup {

private:


    GrassGroup(const GrassGroup&);
    GrassGroup& operator=(const GrassGroup&);

public:

    GLushort m_numTriangles;
    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;
    FloatVector m_vertices;
    UshortVector m_indices;
    Texture* m_grassTexture;

    GrassGroup() {}
    ~GrassGroup();
};



class Grass{

private:

    const int NUM_GRASS_GROUPS;



    /*
      INSTANCE VARIABLES.
    */


    ShaderProgram* m_shader;

    std::vector<GrassGroup*> m_grassGroups;

    Vector4f m_lightPosition;

    void AddQuad(GrassGroup* grassGroup,
	     const Vector3f& bottomLeft, const Vector3f& bottomRight,
		 const Vector3f& topLeft, const Vector3f& topRight);

    void MakeGrass();

    void MakeGrassBlade(GrassGroup* grassGroup,
			const Vector3f& vertexPosition,
			const int lod,
			const float grassHeight, const float grassWidth,
			const Vector3f& windDirection

	);

public:

    Grass(const std::vector<std::string>& textureFilenames, const Vector4f& lightPosition);
    ~Grass();

    void Draw(const Camera& camera);

    void Update(const float delta);

    virtual void MakeGrass(std::vector<GrassBlade>& blades) = 0;

    void Init();

};
