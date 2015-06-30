#pragma once

#include "math/vector3f.hpp"
#include "common.hpp"

class Matrix4f;

class Camera {

private:

    Vector3f m_position;
    Vector3f m_viewDir;
    Vector3f m_right;
    Vector3f m_up;

    std::unique_ptr<Matrix4f> m_projectionMatrix;
    std::unique_ptr<Matrix4f> m_viewMatrix;

    void ComputeViewMatrix();

public:

    Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir);

    Matrix4f GetMvp() const;

    Matrix4f GetViewMatrix() const;

    Vector3f GetPosition() const;

    void Walk(const float amount);
    void Stride(const float amount);
    void Fly(const float amount);

    void HandleInput();



};
