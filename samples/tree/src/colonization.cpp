#include "colonization.hpp"

#include "ewa/random.hpp"

#include <assert.h>

#include <limits.h>

Colonization::Colonization(): m_rng(1) {
    GenerateLeaves();
}


Colonization::~Colonization() {
    for(Branch* branch : m_branches) {
	delete branch;
    }

    for(Leaf* leaf : m_leaves) {
	delete leaf;
    }
}

void Colonization::GenerateLeaves() {
    static constexpr int NUM_LEAVES = 20;

    m_leaves.reserve(NUM_LEAVES);


/*    for(int i = 0; i < NUM_LEAVES; ++i) {
	AddLeaf(Vector3f(
	    m_rng.RandomFloat(-4.0f, +4.0f),
	    m_rng.RandomFloat(2.0f, +3.0f),
	    m_rng.RandomFloat(-4.0f, +4.0f)
		    ));
		    }*/

    AddLeaf(Vector3f(
	     +4.0f,
	     2.5f,
	     -2.0f
		    ));


    for (float y = 0.0; y < 8.0f; y += GROW_DISTANCE) {

	int parent = (y > 0) ? m_branches.size()-1 : -1;
	addBranch(Vector3f(0.0f,y,0.0f), parent);
    }

    computeBranchSizes(0);
}

void Colonization::addBranch(const Vector3f& position,
			     const int parent) {

    Branch* branch = new Branch(position, parent);

    m_branches.push_back(branch);
//    int index = m_branches.size();  // branch index +1


}

Branch::Branch(const Vector3f& position,
	       int parent) {
    m_position = position;
    m_parentIndex = parent;
    m_growDir = Vector3f(0, 0, 0);
    m_growCount = 0;
    m_childCount = 0;
    m_area = TWIG_AREA;
    m_path = -1;
}

void Colonization::computeBranchSizes(int)  {
    // zero all the branch areas
    for (int j = m_branches.size()-1; j >= 0; j--)
    {
	Branch* branch = GetBranch(j);
	branch->m_area = 0.0;
	branch->m_path = -1;
	branch->m_pathArea = -1;
    }

    // starting from tips, set parent area to sum of child areas.
    for (int j = m_branches.size()-1; j >= 0; j--)
    {
	Branch* branch = GetBranch(j);
	if (branch->m_area < TWIG_AREA)
	{
	    branch->m_area = TWIG_AREA;
/*      if (partial && j > lastCount)
        branch->m_area *= growth; */
	}

	if (branch->m_parentIndex != -1) // if not reached root of tree.
	{
	    Branch* parent = GetBranch(branch->m_parentIndex);
	    parent->m_area += branch->m_area;

	}
    }

    // starting from tips, find the thickest child for each parent.
    // j hides the upper j, should it really be like this???


    ////
    for (int j = m_branches.size()-1; j >= 0; j--)
    {
	Branch* branch = GetBranch(j);

	if (branch->m_parentIndex != -1) // if not reached root of tree.
	{
	    Branch* parent = GetBranch(branch->m_parentIndex);
	    if (branch->m_area > parent->m_pathArea)
	    {
		parent->m_pathArea = branch->m_area;
		parent->m_path = j;
	    }
	}
    }


}

Branch* Colonization::GetBranch(const size_t i) {
    assert(i < m_branches.size());

    return m_branches[i];
}

Leaf* Colonization::GetLeaf(const size_t i) {

    assert(i < m_leaves.size());



    return m_leaves[i];
}

size_t Colonization::GetBranchCount() const{
    return m_branches.size();
}

size_t Colonization::GetLeafCount() const{
    return m_leaves.size();
}


bool Colonization::Colonize()
{
  bool active = false;

  int branchCount = m_branches.size();

  // for each leaf, find the closest branch point within the cutoff radius
  for (int i = m_leaves.size()-1; i >= 0; i--)
  {
    Leaf* leaf = GetLeaf(i);
    if (!leaf->m_active)
      continue;

    // the closest branch will be put in m_closest.
    leaf->m_active = FindClosestBranch(leaf->m_position, leaf->m_closest);

    // update growth of closest branch
    if (leaf->m_closest != -1)
    {
      Branch* branch = GetBranch(leaf->m_closest);

      // calculate vector to branch
      Vector3f dir(leaf->m_position);
      dir -= branch->m_position;     //      dir.subtract(branch.m_position);
      dir.Normalize();

      // add normalized vector
      branch->m_growDir += dir;  // branch.m_growDir.add(dir);
      branch->m_growCount++;
    }
  }

  // for each branch
  for (int j = 0; j < branchCount; j++)
  {
    Branch* branch = GetBranch(j);
    // if there's growth, add a new branch
    if (branch->m_growCount != 0 && branch->m_childCount < 3)
    {
      // normalize direction
      branch->m_growDir.Normalize();
      branch->m_growDir *= GROW_DISTANCE; //branch.m_growDir.scale(GROW_DISTANCE);

      // create a new branch
      Vector3f pt(branch->m_position);
      pt += branch->m_growDir;//pt.add(branch.m_growDir);
      addBranch(pt, j);

      branch->m_childCount++; // invalid read of size 4
      active = true;
    }

    // reset growth vector on branch
    branch->m_growDir = Vector3f(0, 0, 0); // Invalid write of size 8
    branch->m_growCount = 0; // Invalid write of size 4
  }


    computeBranchSizes(0);

  return active;
}

void Colonization::AddLeaf(const Vector3f& position) {
  m_leaves.push_back(new Leaf(position));
}

Leaf::Leaf(const Vector3f& position) {
    m_active = true;
    m_position = position;
    m_closest = -1;
}

bool Colonization::FindClosestBranch(const Vector3f& leafPosition, int& closest) {

    closest = -1;
    float closestDist = (float)INT_MAX;

    for(size_t i = 0; i < m_branches.size(); ++i) {
	const Branch* branch = m_branches[i];

	const Vector3f dir = leafPosition - branch->m_position;
	const float dist = dir.Length();
	if(dist < MIN_DISTANCE) {
	    return false; // leaf has reached branch. deactivate it.
	}


	if (dist <= MAX_DISTANCE)
	{
            if (dist < closestDist)
            {
		closest = i;
		closestDist = dist; // update closest branch.
            }
	}


    }

    // leaf is still active.
    return true;

}
