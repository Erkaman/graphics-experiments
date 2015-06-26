#include "camera.hpp"

#include "math/matrix4f.hpp"

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir): m_position(position), m_viewDir(viewDir){

    m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    m_up = Vector3f::Cross(m_right, m_viewDir).Normalize();

    m_projectionMatrix = std::unique_ptr<Matrix4f>(new Matrix4f(Matrix4f::CreatePerspective(90.0f, (float)windowWidth/(float)windowHeight, 0.01f,10000.0f)));


    ComputeViewMatrix();
}

Matrix4f Camera::GetMvp() const {
    return (*m_projectionMatrix) * (*m_viewMatrix);
}

void Camera::ComputeViewMatrix() {
    m_viewMatrix = std::unique_ptr<Matrix4f>(new Matrix4f(
						 Matrix4f::CreateLookAt(
						     m_position,
						     m_position + m_viewDir,
						     m_up)));
}

void Camera::Walk(const double amount) {
    m_position += amount * m_viewDir;
    ComputeViewMatrix();
}

void Camera::Stride(const double amount) {
    m_position += amount * m_right;
    ComputeViewMatrix();
}
