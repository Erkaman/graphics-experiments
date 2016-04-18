#pragma once

#include "icamera.hpp"

// camera control point
struct CameraCP {
    Vector3f m_position;
    Vector3f m_viewDir;
    float m_velocity;

    CameraCP(const Vector3f& position, const Vector3f& viewDir, float velocity):m_position(position), m_viewDir(viewDir), m_velocity(velocity) {

    }
};

class Camera : public ICamera{

private:

    Vector3f m_position;
    Vector3f m_viewDir;
    Vector3f m_right;
    Vector3f m_up;

    float m_ratio;

    void ComputeViewMatrix();
    void ComputeProjection(float near, float far);

    std::vector<CameraCP> m_cps;

    bool m_useCp;

    // interpolation parameter.
    float m_t;


    void Interpolate(const float delta);

    // inti with camera CP
    void InitCamera(const CameraCP& cp);

public:

    Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir);


    virtual void Update(const float delta);

    virtual Vector3f GetPosition() const;
    Vector3f GetUp() const;
    Vector3f GetRight() const;

    virtual Vector3f GetViewDir() const;

    void Walk(const float amount);
    void Stride(const float amount);
    void Fly(const float amount);

    ICamera* CreateReflectionCamera()const;

    void PrintState();




};
