#pragma once

#include "gl/texture.hpp"

#include "random.hpp"

class RandomTexture : public Texture{
public:
    RandomTexture(GLsizei size, Seed seed);

};
