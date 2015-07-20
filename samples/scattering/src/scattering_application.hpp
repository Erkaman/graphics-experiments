#pragma once

#include "application.hpp"
#include "common.hpp"

class Texture;

class Camera;
class FBO;
class Quad;
class Skydome;

class ScatteringApplication : public Application{

private:

    Camera* m_camera;

    FBO* m_fullscreenFbo;

    Quad* m_quad;

    ShaderProgram* m_postShader;

    Skydome* m_skydome;

public:

    ScatteringApplication(int argc, char *argv[]);
    ~ScatteringApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
