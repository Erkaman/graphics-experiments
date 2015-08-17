#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Plane;
class Grass;
class FBO;
class ShaderProgram;
class Quad;

class GrassApplication : public Application{

private:

    Camera* m_camera;
    Plane* m_plane;
    Grass* m_grass;
    FBO* m_fbo1;
    FBO* m_fbo2;

    ShaderProgram* m_postShader;
    Quad* m_quad;


public:

    GrassApplication(int argc, char *argv[]);
    ~GrassApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
};
