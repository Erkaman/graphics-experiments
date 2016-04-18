#pragma once

#include "math/vector3f.hpp"

class AABB {

public:

    Vector3f m_min;
    Vector3f m_max;

    AABB(Vector3f min_, Vector3f max_):m_min(min_), m_max(max_) {}

    AABB() {}



};
