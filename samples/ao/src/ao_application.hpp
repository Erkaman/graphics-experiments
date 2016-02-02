#pragma once


#include "ewa/application.hpp"

#include "ewa/math/vector4f.hpp"

/*#include "ewa/common.hpp"

#include "ewa/math/matrix4f.hpp"

#include <LinearMath/btQuaternion.h>

#include "gui_listener.hpp"

#include <map>
*/


class ICamera;
class IGeometryObject;

/*class HeightMap;
class Skydome;
class Grass;
class Sound;
class ParticleSystem;
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
*/

class AoApplication : public Application{

private:

    ICamera* m_freeCamera;
    ICamera* m_curCamera; // current camera.


    Vector4f m_lightDirection;

    float m_totalDelta;

//    std::map<unsigned int,IGeometryObject*> m_geoObjs;


    /*
    IGeometryObject* LoadObj(const std::string& path, const Vector3f& position,
			     const btQuaternion& rotation = btQuaternion::getIdentity(), float scale = 1.0f);
    */

public:

    AoApplication(int argc, char *argv[]);
    ~AoApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
    void Cleanup() override;

};
