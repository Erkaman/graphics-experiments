#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Skybox;

class SkyboxApplication : public Application{

private:

    Camera* m_camera;
    Skybox* m_skybox;

public:

    SkyboxApplication(int argc, char *argv[]);
    ~SkyboxApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
