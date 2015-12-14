#include "icamera.hpp"

Matrix4f ICamera::GetVp() const {
    return (m_projectionMatrix) * (m_viewMatrix);
}

Matrix4f ICamera::GetMvp(const Matrix4f& m) const {
    return (m_projectionMatrix) * (m_viewMatrix) * (m);
}

Matrix4f ICamera::GetViewMatrix() const {
    return m_viewMatrix;
}

Matrix4f ICamera::GetProjectionMatrix() const {
    return m_projectionMatrix;
}
