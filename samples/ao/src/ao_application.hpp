#pragma once


#include "ewa/application.hpp"

#include "ewa/math/vector4f.hpp"

class ICamera;
class IGeometryObject;
class GeometryObject;
class AoGui;

class AoApplication : public Application{

private:

    ICamera* m_freeCamera;
    ICamera* m_curCamera; // current camera.


    Vector4f m_lightDirection;

    float m_totalDelta;

    GeometryObject* m_geoObj;

    AoGui* m_gui;


//    std::map<unsigned int,IGeometryObject*> m_geoObjs;


    void LoadObj(const std::string& path);

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
