#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class HeightMap;
class Skydome;

class TuhuApplication : public Application{

private:

    Camera* m_camera;
    HeightMap* m_heightMap;

    Skydome* m_skydome;

public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
