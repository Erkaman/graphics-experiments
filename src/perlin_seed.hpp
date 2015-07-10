#pragma once

class Texture;
class ShaderProgram;

class PerlinSeed {


private:

    int perm[256];
    float grad[16][3];

    Texture* permTable;
    Texture* gradTable;

    int PermValue(const int i);

public:

    PerlinSeed(unsigned long long seed);
    ~PerlinSeed();

    void Bind(ShaderProgram& shader);

    void Unbind();

};
