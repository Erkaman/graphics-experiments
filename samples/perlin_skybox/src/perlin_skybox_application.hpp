#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Skydome;

class PerlinSkyboxApplication : public Application{

private:

    Camera* m_camera;
    Skydome* m_skydome;

public:

    PerlinSkyboxApplication(int argc, char *argv[]);
    ~PerlinSkyboxApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
