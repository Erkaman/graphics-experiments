#include "camera.hpp"

#include "math/matrix4f.hpp"

#include "math/quat4f.hpp"
#include "mouse.hpp"
#include "math/math_common.hpp"

using std::string;

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir): m_position(position), m_viewDir(viewDir){

    m_viewDir.Normalize();
    m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    m_up = Vector3f(0,1.0f,0);//Vector3f::Cross(m_right, m_viewDir).Normalize();

    m_projectionMatrix = std::unique_ptr<Matrix4f>(new Matrix4f(Matrix4f::CreatePerspective(45.0f, (float)windowWidth/(float)windowHeight, 0.1f,10000.0f)));

    ComputeViewMatrix();

}

Matrix4f Camera::GetMvp() const {

    return (*m_projectionMatrix) * (*m_viewMatrix);
}

void Camera::ComputeViewMatrix() {
    m_viewMatrix = std::unique_ptr<Matrix4f>(new Matrix4f(
						 Matrix4f::CreateLookAt(
						     m_position,
						     (m_position + m_viewDir),
						     m_up)));
}

void Camera::Walk(const float amount) {
    m_position += amount * m_viewDir;

    ComputeViewMatrix();
}

void Camera::Stride(const float amount) {
    m_position += amount * m_right;
    ComputeViewMatrix();
}

void Camera::Fly(const float amount) {
    m_position += amount * m_up;
    ComputeViewMatrix();
}

void Camera::HandleInput() {



    const Mouse& mouse = Mouse::getInstance();

    bool rotated = false;

    if(!FloatEquals(mouse.GetDeltaX(), 0.0f)) {
	m_viewDir.Rotate(mouse.GetDeltaX()*-0.1f, m_up).Normalize();
	rotated = true;
    }

    if(!FloatEquals(mouse.GetDeltaY(), 0.0f)) {
	m_viewDir.Rotate(mouse.GetDeltaY()*-0.1f, m_right).Normalize();
	rotated = true;
    }

    if(rotated) {
	m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
	m_up = Vector3f(0.0f,1.0f,0.0f);//Vector3f::Cross(m_right, m_viewDir).Normalize();
	ComputeViewMatrix();
    }
}

Matrix4f Camera::GetViewMatrix() const {
    return *m_viewMatrix;
}


Vector3f Camera::GetPosition() const {
    return m_position;
}
