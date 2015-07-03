#include "colonization.hpp"

#include "random.hpp"

Colonization::Colonization(): rng(0) {
    GenerateLeaves();
}

void Colonization::GenerateLeaves() {
    static constexpr int NUM_LEAVES = 20;

    leaves.reserve(NUM_LEAVES);


    for(int i = 0; i < NUM_LEAVES; ++i) {
	Leaf leaf;

	leaves.push_back(leaf);
    }
}
