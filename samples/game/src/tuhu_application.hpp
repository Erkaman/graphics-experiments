#pragma once

#include "application.hpp"
#include "common.hpp"

class Camera;
class HeightMap;
class PerlinSeed;
class Skydome;

class TuhuApplication : public Application{

private:

    Camera* m_camera;
    HeightMap* m_heightMap;

    PerlinSeed* m_perlinSeed;

    Skydome* m_skydome;

public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
