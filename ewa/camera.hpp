#pragma once

#include "math/vector3f.hpp"
#include "math/matrix4f.hpp"


class Camera {

private:

    Vector3f m_position;
    Vector3f m_viewDir;
    Vector3f m_right;
    Vector3f m_up;

    Matrix4f m_projectionMatrix;
    Matrix4f m_viewMatrix;

    void ComputeViewMatrix();

public:

    Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir, bool usePerspectiveProjection);

    Matrix4f GetMvpFromM(const Matrix4f& m) const;
    Matrix4f GetMvpFromM() const;
    Matrix4f GetVp() const;
    Matrix4f GetMvpFromMv(const Matrix4f& modelViewMatrix) const;

    Matrix4f GetViewMatrix() const;
    Matrix4f GetModelViewMatrix() const; // with the identity matrix as the model matrix.
    Matrix4f GetModelViewMatrix(const Matrix4f& modelMatrix) const;

    Matrix4f GetProjectionMatrix() const;

    Matrix4f CreateProjectionMatrix(const float fov, const float aspectRatio, const float near, const float far)const;

    Vector3f GetPosition() const;
    Vector3f GetViewDir() const;
    Vector3f GetUp() const;
    Vector3f GetRight() const;

    void Walk(const float amount);
    void Stride(const float amount);
    void Fly(const float amount);

    void HandleInput(const float delta);

};
