#pragma once

#include "ewa/gl/gl_common.hpp"
#include "ewa/math/vector3f.hpp"
#include "colonization.hpp"

class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;

class Tree {

private:

    /*
      INSTANCE VARIABLES.
    */

    FloatVector m_leavesVertices;
    UshortVector m_leavesIndices;
    GLushort m_leavesNumTriangles;

    FloatVector m_treeVertices;
    UshortVector m_treeIndices;
    GLushort m_treeNumTriangles;



    // the position of the beginning of the stem.
    Vector3f m_stemPosition;

    ShaderProgram* m_phongShader;

    VBO* m_leavesVertexBuffer;
    VBO* m_leavesIndexBuffer;

    VBO* m_treeVertexBuffer;
    VBO* m_treeIndexBuffer;


    Texture* m_leafTexture;
    Texture* m_woodTexture;

    Colonization m_colonization;

    /*
      PRIVATE INSTANCE METHODS
    */

    // position is with respect to m_stemPosition
    void AddLeaf(const Vector3f& position);

    void  AddLeafFace(
	const Vector3f& leafPosition,
	const float ax, const float ay, const float az,
	const float bx, const float by, const float bz,
	const float cx, const float cy, const float cz,
	const float dx, const float dy, const float dz,

	const float nx, const float ny, const float nz

	);

    void DrawLeaves(const Camera& camera, const Vector4f& lightPosition);
    void DrawTree(const Camera& camera, const Vector4f& lightPosition);

    void AddBranch(const std::vector<float>& branchWidths, const std::vector<Vector3f>& branchPositions, const int steps = 5);

public:

    /*
      CONSTRUCTORS
    */

    Tree(const Vector3f& position);
    ~Tree();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

};
