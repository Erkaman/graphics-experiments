#pragma once

#include <string>

class Vector2f {

public:

    float x,y;

    Vector2f(const float x_, const float y_);
    Vector2f();

    operator std::string() const;

};
