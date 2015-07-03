#pragma once

#include "math/vector3f.hpp"
#include "random.hpp"

class Branch
{
public:


    Vector3f position; // branch pos
    int m_parentIndex; // index of parent.

    Vector3f m_growDir;

    int m_growCount;
    int m_childCount;

    float m_area;
    float m_pathArea;
    int m_path;

    int m_next;   // chain of branches in octree cell

    Branch() {}
};

struct Leaf {

    bool m_reached;
    Vector3f position;
    int m_closest; // cloest branch index?

    Leaf() {}
};

class Colonization {

private:

    Random rng;

    std::vector<Leaf> leaves;

    void GenerateLeaves();

public:

    Colonization();

};
