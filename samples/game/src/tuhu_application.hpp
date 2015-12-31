#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include <LinearMath/btQuaternion.h>

#include "gui_listener.hpp"

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

constexpr int DEPTH_FBO_TEXTURE_UNIT = 9;
constexpr int PICKING_FBO_TEXTURE_UNIT = 10;

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


    Car* m_car;

    Gui* m_gui;

    DepthFBO* m_depthFbo;
    PickingFBO* m_pickingFbo;


    Vector4f m_lightDirection;

    // used in shadow mapping for rendering all object from the perspective of the light source.
    Matrix4f m_lightProjectionMatrix;
    Matrix4f m_lightViewMatrix;

    float m_totalDelta;

    ViewFrustum* m_viewFrustum;

    PhysicsWorld* m_physicsWorld;

    unsigned int currentObjId;

    std::vector<IGeometryObject*> m_geoObjs;


    IGeometryObject* LoadObj(const std::string& path, const Vector3f& position,
	const btQuaternion& rotation = btQuaternion::getIdentity());

    Matrix4f MakeLightProj()const;


    void StartPhysics();


    void ParseObjs(const std::string& filename);

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
    virtual void ModelAdded(const std::string& filename);
    virtual void CursorSizeChanged();

};
