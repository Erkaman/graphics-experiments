#pragma once

#include <vector>

#include "ewa/random.hpp"

#include "log.hpp"

template<typename T>
class BiasedRandom{

private:

    Random* m_rng;

    std::vector<T> m_items;
    std::vector<float> m_weights;

    float m_totalWeight;

public:

    BiasedRandom(Random* rng, const std::vector<T>& items, const std::vector<float>& weights): m_rng(rng), m_items(items), m_weights(weights) {

	assert(items.size() == weights.size());

	m_totalWeight = 0;
	for(const float f:  weights) {
	    m_totalWeight += f;
	}
    }

    T GetRandomItem() {

	float weightSum = 0;

	for(int i = 0; i < m_items.size(); ++i) {

	    const float rand = m_rng->RandomFloat(0,m_totalWeight);

	    weightSum += m_weights[i];

	    if(rand < weightSum) {
		return m_items[i];
	    }
	}
    }
};

// let us make every cell 5 units wide.
