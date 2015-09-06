#pragma once

#include "ewa/application.hpp"
#include "ewa/common.hpp"

class Camera;
class Plane;
class Cube;

class CellularApplication : public Application{

private:

    Camera* m_camera;
    Plane* m_cellularPlane;
    Plane* m_perlinPlane;
    Cube* m_cube;

public:

    CellularApplication(int argc, char *argv[]);
    ~CellularApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;
};
