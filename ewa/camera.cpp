#include "camera.hpp"

#include "math/matrix4f.hpp"

#include "math/quat4f.hpp"
#include "mouse_state.hpp"
#include "config.hpp"
#include "math/math_common.hpp"

#include "keyboard_state.hpp"
#include "gl/gl_common.hpp"

using std::string;

/*
constexpr float NEAR = 0.1f;
constexpr float FAR = 100000.0f;
*/

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir): m_position(position), m_viewDir(viewDir){

    m_viewDir.Normalize();
    m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    m_up = Vector3f(0,1.0f,0);//Vector3f::Cross(m_right, m_viewDir).Normalize();

    Config& config = Config::GetInstance();

    m_projectionMatrix =
	Matrix4f::CreatePerspective (45.0f, (float)windowWidth/(float)windowHeight, config.GetZNear(), config.GetZFar());

    /*
    float S = 20.0f;

    m_projectionMatrix =
	Matrix4f::CreateOrthographic(-S, +S, -S, +S, -S, +S);
    */

    ComputeViewMatrix();
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

void Camera::Update(const float delta) {



    const MouseState& mouse = MouseState::GetInstance();
    const KeyboardState& kbs = KeyboardState::GetInstance();


    bool rotated = false;


    Config& config = Config::GetInstance();

    if(!config.IsGui() || (config.IsGui() &&  kbs.IsPressed(GLFW_KEY_F))  ) {

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
	cameraSpeed = 40.0f;
    }else {
	cameraSpeed = 1.0f;
    }
}




Vector3f Camera::GetPosition() const {
    return m_position;
}


Vector3f Camera::GetUp() const {
    return m_up;
}

Vector3f Camera::GetRight() const {
    return m_right;
}

Vector3f Camera::GetViewDir() const {
    return m_viewDir;
}
