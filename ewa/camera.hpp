#pragma once

#include "icamera.hpp"

class Camera : public ICamera{

private:

    Vector3f m_position;
    Vector3f m_viewDir;
    Vector3f m_right;
    Vector3f m_up;


    void ComputeViewMatrix();

    Matrix4f CreateProjectionMatrix(const float fov, const float aspectRatio, const float near, const float far)const;


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


};
