#include "camera.hpp"

#include "math/matrix4f.hpp"

#include "math/quat4f.hpp"
#include "mouse.hpp"
#include "math/math_common.hpp"

#include "keyboard_state.hpp"
#include "gl/gl_common.hpp"

using std::string;

constexpr float NEAR = 0.1f;
constexpr float FAR = 10000.0f;

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir,bool usePerspectiveProjection): m_position(position), m_viewDir(viewDir){

    m_viewDir.Normalize();
    m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    m_up = Vector3f(0,1.0f,0);//Vector3f::Cross(m_right, m_viewDir).Normalize();

    m_projectionMatrix =
	usePerspectiveProjection ?
	Matrix4f::CreatePerspective (45.0f, (float)windowWidth/(float)windowHeight, NEAR, FAR) :
	Matrix4f::CreateOrthographic(-windowWidth,+windowWidth, -windowHeight, +windowHeight, -1.0f, 1.0f);

    ComputeViewMatrix();
}


Matrix4f Camera::GetMvp() const {

    return (m_projectionMatrix) * (m_viewMatrix);
}

Matrix4f Camera::GetMvp(const Matrix4f& modelViewMatrix) const {
    return (m_projectionMatrix) * modelViewMatrix;
}


void Camera::ComputeViewMatrix() {
	m_viewMatrix =
		Matrix4f::CreateLookAt(
			m_position,
			(m_position + m_viewDir),
			m_up);
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

void Camera::HandleInput(const float delta) {



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

    const KeyboardState& kbs = KeyboardState::GetInstance();
    static float cameraSpeed = 1.0f;

    if( kbs.IsPressed(GLFW_KEY_W) ) {
	Walk(delta * cameraSpeed);
    }  else if(kbs.IsPressed(GLFW_KEY_S) ) {
	Walk(-delta * cameraSpeed);
    }

    if(kbs.IsPressed(GLFW_KEY_A) ) {
	Stride(-delta * cameraSpeed);
    }else if( kbs.IsPressed(GLFW_KEY_D) ) {
	Stride(+delta * cameraSpeed);
    }

    if(   kbs.IsPressed(GLFW_KEY_O) ) {
	Fly(+delta * cameraSpeed);
    }else if(   kbs.IsPressed(GLFW_KEY_L) ) {
	Fly(-delta * cameraSpeed);
    }

    if(   kbs.IsPressed(GLFW_KEY_M)  ) {
	cameraSpeed = 5.0f;
    }else {
	cameraSpeed = 1.0f;
    }
}

Matrix4f Camera::GetViewMatrix() const {
    return m_viewMatrix;
}

Matrix4f Camera::GetProjectionMatrix() const {
    return m_projectionMatrix;
}


Vector3f Camera::GetPosition() const {
    return m_position;
}


Matrix4f Camera::GetModelViewMatrix() const {
    return m_viewMatrix;
}


Matrix4f Camera::GetModelViewMatrix(const Matrix4f& modelMatrix) const {
    Matrix4f ret =  (m_viewMatrix) * modelMatrix;

//    LOG_I("ret: %s", tos(ret).c_str() );

    return ret;
}
