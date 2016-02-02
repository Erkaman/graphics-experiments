#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include <LinearMath/btQuaternion.h>

#include "gui_listener.hpp"

#include <map>

class ICamera;
class HeightMap;
class Skydome;
class Grass;
class Sound;
class ParticleSystem;
class IGeometryObject;
class DepthFBO;
class ViewFrustum;
class PhysicsWorld;
class Car;
class CarCamera;
class Gui;
class PickingFBO;
class Gbuffer;
class GpuProfiler;
class Line;
class Cube;
class SsaoPass;
class Skybox;
class CubeMapTexture;
class EnvFBO;

constexpr int DEPTH_FBO_TEXTURE_UNIT = 9;
constexpr int PICKING_FBO_TEXTURE_UNIT = 10;
constexpr int ENV_FBO_TEXTURE_UNIT = 11;

class TuhuApplication : public Application, public GuiListener{

private:

    ICamera* m_freeCamera;
    ICamera* m_carCamera;
    ICamera* m_curCamera; // current camera.


    HeightMap* m_heightMap;

    Skydome* m_skydome;

    Grass* m_grass;

    Sound* m_windSound;

    ParticleSystem* m_smoke;
    ParticleSystem* m_snow;
    ParticleSystem* m_fire;

    IGeometryObject* m_selected;

    EnvFBO* m_envFbo;

    Car* m_car;

    Line* m_line;

    Gui* m_gui;

    DepthFBO* m_depthFbo;
    PickingFBO* m_pickingFbo;
    Gbuffer* m_gbuffer;


    Vector4f m_lightDirection;

    // used in shadow mapping for rendering all object from the perspective of the light source.
    /* Matrix4f m_lightProjectionMatrix;
    Matrix4f m_lightViewMatrix;*/
    Matrix4f m_lightVp;

    float m_totalDelta;

    ViewFrustum* m_cameraFrustum;
    ViewFrustum* m_lightFrustum;


    PhysicsWorld* m_physicsWorld;

    unsigned int currentObjId;

    Cube* m_aabbWireframe;

    Skybox* m_skybox;
    CubeMapTexture* m_cubeMapTexture;


    std::map<unsigned int,IGeometryObject*> m_geoObjs;

    IGeometryObject* LoadObj(const std::string& path, const Vector3f& position,
			     const btQuaternion& rotation = btQuaternion::getIdentity(), float scale = 1.0f);

    Matrix4f MakeLightProj(int frameBufferWidth, int frameBufferHeight)const;


    void StartPhysics();


    void ParseObjs(const std::string& filename);

    GpuProfiler* m_gpuProfiler;

    SsaoPass* m_ssaoPass;

    void UpdateMatrices();

public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
    void Cleanup() override;


    void RenderShadowMap();
    void RenderScene();
    void RenderId();


    /*
      Implementation of GuiListener
     */
    virtual void TranslationAccepted();
    virtual void RotationAccepted();
    virtual void ScaleAccepted();

    virtual void ModelAdded(const std::string& filename);
    virtual void CursorSizeChanged();
    virtual void Duplicate();
    virtual void Delete();

    virtual void BakeAo(int samples, int waveLength, int amplitude, float distAttenuation);

};
