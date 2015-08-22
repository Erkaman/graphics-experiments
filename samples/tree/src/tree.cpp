#include "tree.hpp"

#include "ewa/camera.hpp"
#include "colonization.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector4f.hpp"

#include "ewa/common.hpp"

using std::vector;

void GetAxis(

    Vector3f& from,
    Vector3f& to,
    Vector3f& xaxis,
    Vector3f& zaxis)
{
    // get vector from to A to B
    Vector3f yaxis(to);
    yaxis -= from; //yaxis.subtract(from);
    yaxis.Normalize();

    // 1/sin(pi/4) or 1/cos(pi/4)
    // cross products to get axis
    zaxis = Vector3f(1/1.414f, -1/1.414f, 0.0f);
    zaxis = Vector3f::Cross(zaxis, yaxis);//zaxis.cross(yaxis);
    zaxis.Normalize();

    xaxis = yaxis;
    xaxis = Vector3f::Cross(xaxis, zaxis); //xaxis.cross(zaxis);
    xaxis.Normalize();
}


Tree::Tree(const Vector3f& position): m_stemPosition(position) {

    m_leafTexture = new Texture2D("img/leaf.png");
    m_woodTexture = new Texture2D("img/wood.png");

    m_leafTexture->Bind();
    m_leafTexture->SetTextureRepeat();
    m_leafTexture->SetMinFilter(GL_LINEAR);
    m_leafTexture->SetMagFilter(GL_NEAREST);
    m_leafTexture->Unbind();

    m_woodTexture->Bind();
    m_woodTexture->SetTextureRepeat();
    m_woodTexture->SetMinFilter(GL_LINEAR);
    m_woodTexture->SetMagFilter(GL_NEAREST);
    m_woodTexture->Unbind();



    m_phongShader = new ShaderProgram("shader/phong");

    m_leavesVertexBuffer = VBO::CreateInterleaved(
						    vector<GLuint>{3,3,2} // pos, normal, tex
						    );


    m_leavesIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);


    m_treeVertexBuffer = VBO::CreateInterleaved(
						  vector<GLuint>{3,3,2} // pos, normal, tex
						  );


    m_treeIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_colonization.Colonize();


    for(size_t i = 0; i < m_colonization.GetLeafCount(); ++i) {
	Leaf* leaf = m_colonization.GetLeaf(i);
	AddLeaf(leaf->m_position);
    }


    vector<float> branchWidths;
    vector<Vector3f> branchPositions;

    for (int i = m_colonization.GetBranchCount()-1; i >= 0; i--){
	Branch* branch = m_colonization.GetBranch(i);

	LOG_I("branch num %d, pos: %s, parent %d, growDir %s, growCount %d, childCount %d, area: %f, pathArea: %f, path: %d ", i, tos(branch->m_position).c_str(), branch->m_parentIndex,  tos(branch->m_growDir).c_str(), branch->m_growCount, branch->m_childCount, branch->m_area, branch->m_pathArea, branch->m_path  );
    }

    for (int i = m_colonization.GetBranchCount()-1; i >= 0; i--)
    {
	Branch* branch = m_colonization.GetBranch(i);

	if (branch->m_path != -1)
	    continue;  // parent of some other branch
	int child = i;

	branchWidths.clear();
	branchPositions.clear();

	branchPositions.push_back(branch->m_position);
	branchWidths.push_back(0); // make the branch sharp.
/*    branchPts[0] = branch->m_pt;
      branchWidths[0] = 0.0;*/

	// do animation.  if i > lastCount, it's a new branch and must have a parent
/*    if (m_runAnimation && i > m_lastCount)
      {
      Branch* parent = m_tree.getBranch(branch->m_parent);
      mgPoint3 v(branch->m_pt);
      v.subtract(parent->m_pt);
      v.scale(m_animateGrowth);
      v.add(parent->m_pt);
      branchPts[0] = v;
      }*/

	while (branch->m_parentIndex != -1)
	{
	    Branch* parent = m_colonization.GetBranch(branch->m_parentIndex);

	    branchPositions.push_back(parent->m_position);
	    branchWidths.push_back(sqrt(parent->m_area));

/*
  branchPts[count+1] = parent->m_positions;
  branchWidths[count+1] = sqrt(parent->m_area);*/
//      count++;

	    // if this is a root
	    if (parent->m_path != child)
	    {
		// fix base to reflect root, not parent width
		branchWidths[branchWidths.size()-2] = sqrt(branch->m_area); // ???
		break;
	    }
	    child = branch->m_parentIndex;
	    branch = parent;
	}
	if (branchPositions.size() > 0) {
	    AddBranch(branchWidths, branchPositions);
	}
//      addBranch(m_branchVertexes, m_branchIndexes, count+1, BRANCH_SIDES, branchWidths, branchPts);
    }



    m_leavesVertexBuffer->Bind();
    m_leavesVertexBuffer->SetBufferData(m_leavesVertices);
    m_leavesVertexBuffer->Unbind();

    m_leavesIndexBuffer->Bind();
    m_leavesIndexBuffer->SetBufferData(m_leavesIndices);
    m_leavesIndexBuffer->Unbind();
    m_leavesNumTriangles = (GLushort)(m_leavesIndices.size() / 3);



/*    branchWidths.push_back(0.25f);
      branchWidths.push_back(0.20f);
      branchWidths.push_back(0.15f);

      branchPositions.emplace_back(0.0f,0.0f,0.0f);
      branchPositions.emplace_back(0.0f,0.5f,0.0f);
      branchPositions.emplace_back(0.0f,1.0f,0.0f);
*/
//    AddBranch(branchWidths, branchPositions);

    m_treeVertexBuffer->Bind();
    m_treeVertexBuffer->SetBufferData(m_treeVertices);
    m_treeVertexBuffer->Unbind();

    m_treeIndexBuffer->Bind();
    m_treeIndexBuffer->SetBufferData(m_treeIndices);
    m_treeIndexBuffer->Unbind();
    m_treeNumTriangles = (GLushort)(m_treeIndices.size() / 3);

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

    DrawTree(camera, lightPosition);


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

void Tree::DrawTree(const Camera& camera, const Vector4f& lightPosition) {

    m_woodTexture->Bind();

    //m_phongShader->SetPhongUniforms(Matrix4f::CreateTranslation(m_stemPosition), camera, lightPosition);

    VBO::DrawIndices(*m_treeVertexBuffer, *m_treeIndexBuffer, GL_TRIANGLES, (m_treeNumTriangles)*3);

    m_woodTexture->Unbind();
}


void Tree::AddBranch(const std::vector<float>& branchWidths, const std::vector<Vector3f>& branchPositions, const int steps) {
    static const float BRANCH_SCALE = 1.0f/10.0f;

    assert(branchWidths.size() == branchPositions.size());

    GLushort count = (GLushort)branchWidths.size();

    int baseIndex = m_treeVertices.size();
    double radius;
    Vector3f normal, from, to, xaxis, zaxis;
    Vector3f v;

    for (int i = 0; i <= steps; i++)
    {
	float angle = (2*PI*i)/steps;
	float x = cos(angle);
	float z = sin(angle);
	float height = 0.0f;

	// add interior points
	for (int j = count-1; j > 0; j--)
	{
	    // find axis at next segment
	    from = branchPositions[j];
	    to = branchPositions[j-1];
	    radius = branchWidths[j];

	    Vector3f oldNormal(normal);

	    // get normal
	    GetAxis(from, to, xaxis, zaxis);
	    normal.x = x*xaxis.x + z*zaxis.x;
	    normal.y = x*xaxis.y + z*zaxis.y;
	    normal.z = x*xaxis.z + z*zaxis.z;

	    if (j < count-1) // at first iteration not executed.
	    {
		normal += oldNormal;  //normal.add(oldNormal);
		normal *= 0.5f;//normal.scale(0.5);
	    }

	    // set point
	    m_treeVertices.push_back( (float) (from.x + radius * normal.x));
	    m_treeVertices.push_back((float) (from.y + radius * normal.y));
	    m_treeVertices.push_back((float) (from.z + radius * normal.z));

	    // normal faces out
	    m_treeVertices.push_back((float) normal.x);
	    m_treeVertices.push_back( (float) normal.y);
	    m_treeVertices.push_back( (float) normal.z);

	    // texture coordinates
	    m_treeVertices.push_back( (float) (BRANCH_SCALE*2*PI*i)/steps);
	    m_treeVertices.push_back( (float) (BRANCH_SCALE*height));

	    Vector3f delta(to);
	    delta -= from;//delta.subtract(from);
	    height += delta.Length();
	}

	// add the top row
	from = branchPositions[1];
	to = branchPositions[0];
	radius = branchWidths[0];
	GetAxis(from, to, xaxis, zaxis);

	// multiply by axis
	normal.x = x*xaxis.x + z*zaxis.x;
	normal.y = x*xaxis.y + z*zaxis.y;
	normal.z = x*xaxis.z + z*zaxis.z;

	// set point
	m_treeVertices.push_back( (float) (to.x + radius * normal.x));
	m_treeVertices.push_back(  (float) (to.y + radius * normal.y));
	m_treeVertices.push_back( (float) (to.z + radius * normal.z));


	// normal faces out
	m_treeVertices.push_back((float) normal.x);
	m_treeVertices.push_back( (float) normal.y);
	m_treeVertices.push_back( (float) normal.z);


	// texture coordinates
	m_treeVertices.push_back( (float) (BRANCH_SCALE*2*PI*i)/steps);
	m_treeVertices.push_back( (float) (BRANCH_SCALE*height));
    }


    // add the indexes
    for (GLushort i = GLushort(0); i < steps; i++)
    {
	GLushort index = GLushort(baseIndex + i*count);
	for (GLushort j = GLushort(0); j < count-1; j++)
	{
	    m_treeIndices.push_back(index);  // tl
	    m_treeIndices.push_back(index+1);  // bl
	    m_treeIndices.push_back(index+count);  // tr

	    m_treeIndices.push_back(index+1);  // bl
	    m_treeIndices.push_back(index+count+1);  // br
	    m_treeIndices.push_back(index+count);  // tr

	    index++;
	}
    }

}

Tree::~Tree() {
    delete m_phongShader;

    delete m_leavesVertexBuffer;
    delete m_leavesIndexBuffer;

    delete m_treeVertexBuffer;
    delete m_treeIndexBuffer;


    delete m_leafTexture;
    delete m_woodTexture;

}
