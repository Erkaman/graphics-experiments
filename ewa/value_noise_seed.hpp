#pragma once

class Texture;
class ShaderProgram;

class ValueNoiseSeed {


private:

    int perm[256];
    float rand[256];

    Texture* permTable;
    Texture* randomTable;

    int PermValue(const int i);

public:

    ValueNoiseSeed(unsigned long long seed);
    ~ValueNoiseSeed();

    void Bind(ShaderProgram& shader);
    void Unbind();


};
