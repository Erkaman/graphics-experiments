#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Plane;

class GrassApplication : public Application{

private:

    Camera* m_camera;
    Plane* m_plane;

public:

    GrassApplication(int argc, char *argv[]);
    ~GrassApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
};
