#pragma once


#include "ewa/application.hpp"

#include "ewa/math/vector4f.hpp"

class ICamera;
class IGeometryObject;
class GeometryObject;
class AoGui;
class SimpleRender;
class GeometryObjectData;
class RayTracer;

class AoApplication : public Application{

private:

    ICamera* m_freeCamera;
    ICamera* m_curCamera; // current camera.


    Vector4f m_lightDirection;

    float m_totalDelta;

    RayTracer* m_rayTracer;

    AoGui* m_gui;

    GeometryObjectData* m_eobFile;



//    std::map<unsigned int,IGeometryObject*> m_geoObjs;

    SimpleRender* m_render;

public:

    AoApplication(int argc, char *argv[]);
    ~AoApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
    void Cleanup() override;

    void BakeAo();

};
