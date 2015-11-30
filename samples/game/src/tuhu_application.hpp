#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

#include "ewa/math/vector4f.hpp"

class Camera;
class HeightMap;
class Skydome;
class Grass;
class Sound;
class ParticleSystem;
class GeometryObject;
class DepthFBO;

class TuhuApplication : public Application{

private:

    Camera* m_camera;
    HeightMap* m_heightMap;

    Skydome* m_skydome;

    Grass* m_grass;

    Sound* m_windSound;

    ParticleSystem* m_smoke;
    ParticleSystem* m_snow;
    ParticleSystem* m_fire;

    GeometryObject* m_stoneFloor;
    GeometryObject* m_flatWoodFloor;
    GeometryObject* m_woodFloor;
    GeometryObject* m_sphere;
    GeometryObject* m_plane;
    GeometryObject* m_tree;

    DepthFBO* m_depthFbo;

    Vector4f m_lightDirection;

public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

    void RenderShadowMap();
    void RenderScene();

};
