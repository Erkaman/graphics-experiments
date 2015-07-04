#pragma once

#include "math/vector3f.hpp"
#include "random.hpp"

const float TWIG_AREA = 0.1f*0.1f;

const float GROW_DISTANCE = 1.0f;
const float MIN_GROW = 1.0f;
const float MIN_DISTANCE = 2.0f * GROW_DISTANCE;
const float MAX_DISTANCE = 10.0f * GROW_DISTANCE;


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

    bool m_active;
    Vector3f m_position;
    int m_closest; // cloest branch index?

    Leaf(const Vector3f& position);
};

class Colonization {

private:

    Random m_rng;

    std::vector<Leaf> m_leaves;
    std::vector<Branch> m_branches;

    void GenerateLeaves();

    void AddLeaf(const Vector3f& position);

    bool FindClosestBranch(const Vector3f& leafPosition, int& closest);


public:

    Colonization();

    std::vector<Leaf> GetLeaves()const;

    void addBranch(const Vector3f& position,
				 const int parent);

    void computeBranchSizes(int lastCount) ;

    Branch& GetBranch(const size_t i);
    size_t GetBranchCount();
    Leaf& GetLeaf(const size_t i);

    bool Colonize();




};
