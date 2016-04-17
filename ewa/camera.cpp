#include "camera.hpp"

#include "math/matrix4f.hpp"

#include "math/quat4f.hpp"
#include "mouse_state.hpp"
#include "config.hpp"
#include "math/math_common.hpp"

#include "keyboard_state.hpp"
#include "gl/gl_common.hpp"

using std::string;


void Camera::ComputeProjection(float near, float far) {
    m_projectionMatrix =
	Matrix4f::CreatePerspective (45.0f, m_ratio, near, far);
}


/*
constexpr float NEAR = 0.1f;
constexpr float FAR = 100000.0f;
*/

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir, bool useCp): m_position(position), m_viewDir(viewDir), m_useCp(useCp), m_t(0){

    m_viewDir.Normalize();
    m_right = Vector3f::Cross(m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    m_up = Vector3f(0,1.0f,0);//Vector3f::Cross(m_right, m_viewDir).Normalize();

    Config& config = Config::GetInstance();

    m_ratio = (float)windowWidth/(float)windowHeight;

    ComputeProjection(config.GetZNear(), config.GetZFar());


    /*
    float S = 20.0f;

    m_projectionMatrix =
	Matrix4f::CreateOrthographic(-S, +S, -S, +S, -S, +S);
    */

    ComputeViewMatrix();

    /*
    m_cps.push_back(CameraCP(
			Vector3f(-52.844795, 14.650712, 35.442703),
			Vector3f(0.933511, -0.318891, 0.163909)));

    m_cps.push_back(CameraCP(
			Vector3f(-28.865242, 6.459203, 39.653118),
			Vector3f(0.933511, -0.318891, 0.163909)) );

    */
    m_cps.push_back(CameraCP(Vector3f(-38.324429, 11.885699, 37.992260),Vector3f(0.933511, -0.318891, 0.163909)));

    m_cps.push_back( CameraCP(Vector3f(-19.206289, 6.142766, 41.482296),Vector3f(0.965010, 0.064600, 0.254132)) )    ;


    m_cps.push_back( CameraCP(Vector3f(0.156958, 7.438980, 46.581543),Vector3f(0.962155, -0.024363, 0.271412)) );

    m_cps.push_back(  CameraCP(Vector3f(19.496862, 6.949271, 52.037083),Vector3f(0.879356, -0.057421, 0.472689)) );

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

    if(m_useCp) {

	Interpolate(delta);
	return;
    }

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

ICamera* Camera::CreateReflectionCamera()const {
    Camera* reflectionCamera = new Camera(*this);

    reflectionCamera->m_position.y *= -1;
    reflectionCamera->m_viewDir.y *= -1;
    reflectionCamera->m_right = Vector3f::Cross(
	reflectionCamera->m_viewDir, Vector3f(0.0f, 1.0f, 0.0f)).Normalize();
    reflectionCamera->m_up = Vector3f(0,1.0f,0);



    reflectionCamera->ComputeViewMatrix();

    reflectionCamera->ComputeProjection(0.1f, 300.0f);


    return reflectionCamera;
}

void Camera::PrintState() {
    LOG_I("CameraCP(%s,%s)",
	  (string("Vector3f") + string(m_position)).c_str(),
	  (string("Vector3f") + string(m_viewDir)).c_str() );
}
/*PrintState:CameraCP((-52.844795, 14.650712, 35.442703),(0.933511, -0.318891, 0.163909))
INFO: /Users/eric/tuhu/ewa/camera.cpp:173:PrintState:CameraCP((-28.865242, 6.459203, 39.653118),(0.933511, -0.318891, 0.163909))
 */

float Lerp(float cp1, float cp2, float t) {
    return cp1 * (1.0 - t) + cp2 * (t);
}

Vector3f Lerp(const Vector3f& cp1, const Vector3f& cp2, float t ) {
    return Vector3f(
	Lerp(cp1.x, cp2.x, t),
	Lerp(cp1.y, cp2.y, t),
	Lerp(cp1.z, cp2.z, t)
	);
}
/*
Vector2f CatmullRomSpline(const Vector2f &P1,const Vector2f &P2,const Vector2f &P3,const Vector2f &P4,float t)
{
    float t2 = t * t;
    float t3 = t * t2;

    return ((P2 * 2.0f) + (-P1 + P3) * t + (P1 * 2.0f - P2 * 5.0f + P3 * 4.0f - P4) * t2 + (-P1 + P2 * 3.0f - P3 * 3.0f + P4) * t3) * 0.5f;
}
*/


void Camera::Interpolate(const float delta) {
    m_t += delta*0.4;

    static int i_cp = 0;

    if(m_t <= 1.0) {
//	LOG_I("LERP: %f", m_t);

	CameraCP cp1 = m_cps[i_cp+0];
	CameraCP cp2 = m_cps[i_cp+1];

	m_position = Lerp(cp1.m_position, cp2.m_position, m_t);
	m_viewDir = Lerp(cp1.m_viewDir, cp2.m_viewDir, m_t);

//	LOG_I("lerp: %s", string(m_position).c_str() );

	ComputeViewMatrix();

    } else {
	i_cp++;
	m_t -= 1.0f;
    }
}
