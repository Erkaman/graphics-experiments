#pragma once

#include "math/vector3f.hpp"

class AABB {

public:

    Vector3f min;
    Vector3f max;

    AABB(Vector3f min_, Vector3f max_):min(min_), max(max_) {}

    AABB() {}



};
