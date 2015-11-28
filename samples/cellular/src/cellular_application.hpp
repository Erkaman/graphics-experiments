#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

#include "ewa/math/vector4f.hpp"


class Camera;
class Cube;
class GeometryObject;

class CellularApplication : public Application{

private:

    Camera* m_camera;
    Cube* m_cube;

    GeometryObject* m_stoneFloor;
    GeometryObject* m_flatWoodFloor;
    GeometryObject* m_woodFloor;

    GeometryObject* m_sun;
    GeometryObject* m_ball;

    Vector4f m_lightPosition;

    float m_time;


public:

    CellularApplication(int argc, char *argv[]);
    ~CellularApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
};
