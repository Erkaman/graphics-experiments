#pragma once

class Vector2f;

class ValueNoise {
private:

    int m_perm[512];
    float m_rand[512];

    float GetRand(int i)const;
    int GetPerm(int i)const;

public:

    ValueNoise(unsigned long long seed);

    float Sample(const Vector2f& p)const;
    float Turbulence(const int octaves, const Vector2f& P, const float lacunarity, const float gain)const;

};
