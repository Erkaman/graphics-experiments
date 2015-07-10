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

    Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir);
	
    Matrix4f GetMvp() const;
    Matrix4f GetMvp(const Matrix4f& modelViewMatrix) const;

    Matrix4f GetViewMatrix() const;
    Matrix4f GetModelViewMatrix() const; // with the identity matrix as the model matrix.
    Matrix4f GetModelViewMatrix(const Matrix4f& modelMatrix) const;

    Matrix4f GetProjectionMatrix() const;

    Vector3f GetPosition() const;

    void Walk(const float amount);
    void Stride(const float amount);
    void Fly(const float amount);

    void HandleInput();



};
