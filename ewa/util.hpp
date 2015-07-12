#pragma once

#include "random.hpp"

template<typename RandomAccessIterator>
void Shuffle(Random& random, RandomAccessIterator begin, RandomAccessIterator end) {
    for(size_t n = end - begin - 1; n >= 1; --n) {
	size_t k = random.Next() % (n + 1);
	if(k != n) {
	    std::iter_swap(begin + k, begin + n);
	}
    }
}
