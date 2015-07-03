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
}

std::vector<Leaf> Colonization::GetLeaves()const {
    return m_leaves;
}
