#pragma once

class Texture;
class ShaderProgram;

class WorleySeed {


private:

    int perm[256];
    float grad[16][3];

    Texture* permTable;
    Texture* vectorTable;

    int PermValue(const int i);

public:

    WorleySeed(unsigned long long seed);
    ~WorleySeed();

    void Bind(ShaderProgram& shader);

    void Unbind();
};
