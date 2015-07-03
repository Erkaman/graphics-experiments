#pragma once

#include "math/vector3f.hpp"
#include "random.hpp"

const double TWIG_AREA = 0.1*0.1;

const double GROW_DISTANCE = 1.0;
const double MIN_GROW = 1.0;
const double MIN_DISTANCE = 2 * GROW_DISTANCE;
const double MAX_DISTANCE = 10 * GROW_DISTANCE;


class Branch
{
public:


    Vector3f m_position; // branch pos
    int m_parentIndex; // index of parent.

    Vector3f m_growDir;

    int m_growCount;
    int m_childCount;

    float m_area;
    float m_pathArea;
    int m_path;

    int m_next;   // chain of branches in octree cell

    Branch(const Vector3f& position,
	   int parent);
};

struct Leaf {

    bool m_reached;
    Vector3f m_position;
    int m_closest; // cloest branch index?

    Leaf() {}
};

class Colonization {

private:

    Random m_rng;

    std::vector<Leaf> m_leaves;
    std::vector<Branch> m_branches;

    void GenerateLeaves();

public:

    Colonization();

    std::vector<Leaf> GetLeaves()const;

    void addBranch(const Vector3f& position,
				 const int parent);

    void computeBranchSizes(int lastCount) ;

    Branch& GetBranch(int i);
    size_t GetBranchCount();


};
