#pragma once

#include "random.hpp"

#include "gl/gl_common.hpp"

template<typename RandomAccessIterator>
void Shuffle(Random& random, RandomAccessIterator begin, RandomAccessIterator end) {
    for(size_t n = end - begin - 1; n >= 1; --n) {
	size_t k = random.Next() % (n + 1);
	if(k != n) {
	    std::iter_swap(begin + k, begin + n);
	}
    }
}

class VBO;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );
