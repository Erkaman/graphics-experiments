#include "colonization.hpp"

#include "random.hpp"

Colonization::Colonization(): m_rng(1) {
    GenerateLeaves();
}

void Colonization::GenerateLeaves() {
    static constexpr int NUM_LEAVES = 20;

    m_leaves.reserve(NUM_LEAVES);


    for(int i = 0; i < NUM_LEAVES; ++i) {
	Leaf leaf;

	leaf.m_position = Vector3f(
	    m_rng.RandomFloat(-4.0, +4.0),
	    m_rng.RandomFloat(2.0, +3.0),
	    m_rng.RandomFloat(-4.0, +4.0)
	    );

	m_leaves.push_back(leaf);
    }


    for (double y = 0.0; y < 8.0f; y += GROW_DISTANCE) {

	int parent = (y > 0) ? m_branches.size()-1 : -1;
	addBranch(Vector3f(0,y,0), parent);
    }

    computeBranchSizes(0);

}

std::vector<Leaf> Colonization::GetLeaves()const {
    return m_leaves;
}

void Colonization::addBranch(const Vector3f& position,
			     const int parent) {

    Branch branch(position, parent);

    m_branches.push_back(branch);
    int index = m_branches.size();  // branch index +1

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

void Colonization::computeBranchSizes(int lastCount)  {
    // zero all the branch areas
    for (int j = m_branches.size()-1; j >= 0; j--)
    {
	Branch& branch = GetBranch(j);
	branch.m_area = 0.0;
	branch.m_path = -1;
	branch.m_pathArea = -1;
    }

    // starting from tips, set parent area to sum of child areas.
    for (int j = m_branches.size()-1; j >= 0; j--)
    {
	Branch& branch = GetBranch(j);
	if (branch.m_area < TWIG_AREA)
	{
	    branch.m_area = TWIG_AREA;
/*      if (partial && j > lastCount)
        branch->m_area *= growth;
	}*/

	    if (branch.m_parentIndex != -1) // if not reached root of tree.
	    {
		Branch& parent = GetBranch(branch.m_parentIndex);
		parent.m_area += branch.m_area;
	    }
	}

	// starting from tips, find the thickest child for each parent.
	for (int j = m_branches.size()-1; j >= 0; j--)
	{
	    Branch& branch = GetBranch(j);

	    if (branch.m_parentIndex != -1) // if not reached root of tree.
	    {
		Branch& parent = GetBranch(branch.m_parentIndex);
		if (branch.m_area > parent.m_pathArea)
		{
		    parent.m_pathArea = branch.m_area;
		    parent.m_path = j;
		}
	    }
	}

    }
}

Branch& Colonization::GetBranch(int i) {
    return m_branches[i];
}

size_t Colonization::GetBranchCount() {
    return m_branches.size();
}
