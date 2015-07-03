#include "tree.hpp"

#include "common.hpp"
#include "camera.hpp"
#include "colonization.hpp"

#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"

#include "math/matrix4f.hpp"
#include "math/vector4f.hpp"

using std::vector;

Tree::Tree(const Vector3f& position): m_stemPosition(position) {

    m_leafTexture = std::make_unique<Texture2D>("img/leaf.png");

    m_leafTexture->Bind();
    m_leafTexture->SetTextureTiling();
    m_leafTexture->SetMinFilter(GL_LINEAR);
    m_leafTexture->SetMagFilter(GL_NEAREST);
    m_leafTexture->Unbind();

    m_phongShader = std::make_unique<ShaderProgram>("shader/phong");

    m_leavesVertexBuffer = std::unique_ptr<VBO>(VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_NORMAL_ATTRIB_INDEX,
							    VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{3,3,2}
						    ));


    m_leavesIndexBuffer = std::unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_SHORT));


    m_treeVertexBuffer = std::unique_ptr<VBO>(VBO::CreateInterleaved(
						    vector<GLuint>{
							VBO_POSITION_ATTRIB_INDEX,
							    VBO_NORMAL_ATTRIB_INDEX,
							    VBO_TEX_COORD_ATTRIB_INDEX},
						    vector<GLuint>{3,3,2}
						    ));


    m_treeIndexBuffer = std::unique_ptr<VBO>(VBO::CreateIndex(GL_UNSIGNED_SHORT));



    // DO REMEMBER TO RESERVE MEMORY IN VERTEX VECTOR.
    AddLeaf(Vector3f(0,4,0));

    AddLeaf(Vector3f(3,4,0));

    m_leavesVertexBuffer->Bind();
    m_leavesVertexBuffer->SetBufferData(m_leavesVertices);
    m_leavesVertexBuffer->Unbind();

    m_leavesIndexBuffer->Bind();
    m_leavesIndexBuffer->SetBufferData(m_leavesIndices);
    m_leavesIndexBuffer->Unbind();
    m_leavesNumTriangles = (GLushort)(m_leavesIndices.size() / 3);
}

void Tree::AddLeaf(const Vector3f& position) {

    const Vector3f leafPosition = position;

    AddLeafFace(
	leafPosition,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.0f,0.0f,1.0
	);

    AddLeafFace(
	leafPosition,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.0f,0.0f, -1.0f
	);

    AddLeafFace(
	leafPosition,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.0f,1.0,0.0f
	);


    AddLeafFace(
	leafPosition,
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.0f,-1.0,0.0f

	);

    AddLeafFace(
	leafPosition,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	1.0f, 0.0f, 0.0f
	);

    AddLeafFace(
	leafPosition,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-1.0f, 0.0f, 0.0f
	);


}

void  Tree::AddLeafFace(
    const Vector3f& leafPosition,
    const float ax, const float ay, const float az,
    const float bx, const float by, const float bz,
    const float cx, const float cy, const float cz,
    const float dx, const float dy, const float dz,

    const float nx, const float ny, const float nz

    ) {
    const GLushort base = (GLushort)(m_leavesVertices.size() / 8);


    constexpr float SCALE = 0.2f;

    m_leavesVertices.push_back(ax*SCALE+leafPosition.x);
    m_leavesVertices.push_back(ay*SCALE+leafPosition.y);
    m_leavesVertices.push_back(az*SCALE+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(0); // texcoord
    m_leavesVertices.push_back(0);


    m_leavesVertices.push_back(bx*SCALE+leafPosition.x);
    m_leavesVertices.push_back(by*SCALE+leafPosition.y);
    m_leavesVertices.push_back(bz*SCALE+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(1); // texcoord
    m_leavesVertices.push_back(0);



    m_leavesVertices.push_back(cx*SCALE+leafPosition.x);
    m_leavesVertices.push_back(cy*SCALE+leafPosition.y);
    m_leavesVertices.push_back(cz*SCALE+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(0); // texcoord
    m_leavesVertices.push_back(1);



    m_leavesVertices.push_back(dx*SCALE+leafPosition.x);
    m_leavesVertices.push_back(dy*SCALE+leafPosition.y);
    m_leavesVertices.push_back(dz*SCALE+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(1); // texcoord
    m_leavesVertices.push_back(1);



    m_leavesIndices.push_back(base + 2);
    m_leavesIndices.push_back(base + 1);
    m_leavesIndices.push_back(base);


    m_leavesIndices.push_back(base + 3);
    m_leavesIndices.push_back(base + 2);
    m_leavesIndices.push_back(base);

}


void Tree::Draw(const Camera& camera, const Vector4f& lightPosition) {

    m_phongShader->Bind();

    DrawLeaves(camera, lightPosition);

    m_phongShader->Unbind();

}

void Tree::DrawLeaves(const Camera& camera, const Vector4f& lightPosition) {
    // next load texture.
    m_leafTexture->Bind();
    m_phongShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);

    // setup matrices.

    m_phongShader->SetPhongUniforms(Matrix4f::CreateTranslation(m_stemPosition) , camera, lightPosition);

    VBO::DrawIndices(*m_leavesVertexBuffer, *m_leavesIndexBuffer, GL_TRIANGLES, (m_leavesNumTriangles)*3);

    m_leafTexture->Unbind();
}
