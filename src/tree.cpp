#include "tree.hpp"

#include "common.hpp"
#include "camera.hpp"

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


    AddLeaf(Vector3f(0,3,0));

    m_leavesVertexBuffer->Bind();
    m_leavesVertexBuffer->SetBufferData(m_leavesVertices);
    m_leavesVertexBuffer->Unbind();

    m_leavesIndexBuffer->Bind();
    m_leavesIndexBuffer->SetBufferData(m_leavesIndices);
    m_leavesIndexBuffer->Unbind();
    m_leavesNumTriangles = (GLushort)(m_leavesIndices.size() / 3);
}

void Tree::AddLeaf(const Vector3f& position) {

    const Vector3f leafPosition = position + m_stemPosition;

    AddLeafFace(
    leafPosition,
    0.5f, 0.5f, 0.5f,
	    0.5f, -0.5f, 0.5f,
	    -0.5f, -0.5f, 0.5f,
	    -0.5f, 0.5f, 0.5f,
	0.0f,0.0f,1.0
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

    m_leavesVertices.push_back(ax+leafPosition.x);
    m_leavesVertices.push_back(ay+leafPosition.y);
    m_leavesVertices.push_back(az+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(0); // texcoord
    m_leavesVertices.push_back(0);


    m_leavesVertices.push_back(bx+leafPosition.x);
    m_leavesVertices.push_back(by+leafPosition.y);
    m_leavesVertices.push_back(bz+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(1); // texcoord
    m_leavesVertices.push_back(0);



    m_leavesVertices.push_back(cx+leafPosition.x);
    m_leavesVertices.push_back(cy+leafPosition.y);
    m_leavesVertices.push_back(cz+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(0); // texcoord
    m_leavesVertices.push_back(1);



    m_leavesVertices.push_back(dx+leafPosition.x);
    m_leavesVertices.push_back(dy+leafPosition.y);
    m_leavesVertices.push_back(dz+leafPosition.z);
    m_leavesVertices.push_back(nx);
    m_leavesVertices.push_back(ny);
    m_leavesVertices.push_back(nz);
    m_leavesVertices.push_back(1); // texcoord
    m_leavesVertices.push_back(1);


    m_leavesIndices.push_back(base);
    m_leavesIndices.push_back(base + 1);
    m_leavesIndices.push_back(base + 2);
    m_leavesIndices.push_back(base);
    m_leavesIndices.push_back(base + 2);
    m_leavesIndices.push_back(base + 3);
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

    const Matrix4f modelViewMatrix = camera.GetModelViewMatrix(Matrix4f::CreateTranslation(m_stemPosition) );
    const Matrix4f mvp = camera.GetMvp(modelViewMatrix);

    m_phongShader->SetUniform("mvp", mvp);
    m_phongShader->SetUniform("modelViewMatrix", modelViewMatrix);
    m_phongShader->SetUniform("normalMatrix", Matrix4f::GetNormalMatrix(modelViewMatrix));
    m_phongShader->SetUniform("viewSpaceLightPosition", Vector3f(camera.GetViewMatrix() * lightPosition) );


    m_leavesVertexBuffer->EnableVertexAttribInterleavedWithBind();

    m_leavesIndexBuffer->Bind();

    // DRAW.
    m_leavesIndexBuffer->DrawIndices(GL_TRIANGLES, (m_leavesNumTriangles)*3);

    // unsetup vertex buffer.

    m_leavesIndexBuffer->Unbind();

    m_leavesVertexBuffer->DisableVertexAttribInterleavedWithBind();



    m_leafTexture->Unbind();


}
