#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"

#include "ewa/math/vector3f.hpp"

struct GrassBlade {
    Vector3f vertexPosition;
    float grassHeight;
    float grassWidth;
    Vector3f windDirection;
    int lod;
};

class Grass{

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    ShaderProgram* m_shader;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    Texture* m_grassTexture;

    void AddQuad(FloatVector& vertices, UshortVector& indices,
	     const Vector3f& bottomLeft, const Vector3f& bottomRight,
		 const Vector3f& topLeft, const Vector3f& topRight);

    void MakeGrass();

    void MakeGrassBlade(FloatVector& vertices, UshortVector& indices,
			const Vector3f& vertexPosition,
			const int lod,
			const float grassHeight, const float grassWidth,
			const Vector3f& windDirection

	);


public:

    Grass(const std::string& textureFilename);
    ~Grass();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

    virtual void MakeGrass(std::vector<GrassBlade>& blades) = 0;

    void Init();

};
