#include "camera.hpp"

#include "math/matrix4f.hpp"

#include "math/quat4f.hpp"
#include "mouse_state.hpp"
#include "config.hpp"
#include "math/math_common.hpp"

#include "keyboard_state.hpp"
#include "gl/gl_common.hpp"

using std::string;


void Camera::ComputeProjection(float zNear, float zFar) {
    m_projectionMatrix =
	Matrix4f::CreatePerspective (45.0f, m_ratio, zNear, zFar);
}


/*
constexpr float NEAR = 0.1f;
constexpr float FAR = 100000.0f;
*/


void Camera::InitCamera(const CameraCP& cp) {
    m_position = cp.m_position;
    m_viewDir = cp.m_viewDir;
}

Camera::Camera(const int windowWidth, const int windowHeight, const Vector3f& position, const Vector3f& viewDir): m_position(position), m_viewDir(viewDir), m_t(0){


float FAST_SPEED = 0.8;
float NORMAL_SPEED = 0.4;

    float SLOW = 0.2f;
    float NORMAL= 0.4;


   InitCamera(

       CameraCP(Vector3f(-3.693598, 9.265701, -2.659110),Vector3f(0.681054, -0.480876, 0.552199), 0.24));


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

    m_useCp = false;

// torch.
m_cps.push_back(CameraCP(Vector3f(-76.706909, 18.342823, -37.618912),Vector3f(0.000293, -0.160539, -0.987029), SLOW));
m_cps.push_back(CameraCP(Vector3f(-73.814270, 14.097636, -63.895123),Vector3f(-0.032617, -0.091302, -0.995289), SLOW));
m_cps.push_back(CameraCP(Vector3f(-74.709152, 11.592713, -91.201576),Vector3f(-0.030332, -0.206862, -0.977900), SLOW));
m_cps.push_back(CameraCP(Vector3f(-75.336548, 7.314081, -111.428047),Vector3f(-0.030332, -0.206862, -0.977900), SLOW));
m_cps.push_back(CameraCP(Vector3f(-75.120773, 6.481238, -120.870163),Vector3f(0.022758, -0.087841, -0.995874), SLOW));
m_cps.push_back(CameraCP(Vector3f(-91.377907, 34.241344, -136.638443),Vector3f(-0.698802, -0.259944, -0.666412), SLOW));
m_cps.push_back(CameraCP(Vector3f(-98.991989, 31.997124, -143.899582),Vector3f(-0.698802, -0.259944, -0.666412), SLOW));
m_cps.push_back(CameraCP(Vector3f(-102.237778, 30.670305, -146.870285),Vector3f(-0.749026, -0.215933, -0.626365), SLOW));
m_cps.push_back(CameraCP(Vector3f(-102.237778, 30.670305, -146.870285),Vector3f(-0.749026, -0.215933, -0.626365), SLOW));


// deferred lighting.
m_cps.push_back(CameraCP(Vector3f(-92.013481, 53.859867, -150.773727),Vector3f(0.881673, -0.312643, 0.353423), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-43.074444, 51.202881, -119.976768),Vector3f(0.668680, -0.250739, 0.699997), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-21.855598, 57.479309, -76.467361),Vector3f(0.319121, -0.228934, 0.919647), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-24.527514, 64.626579, -28.037077),Vector3f(-0.376258, -0.325958, 0.867284), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-63.207527, 47.784172, 25.211224),Vector3f(-0.686603, -0.294466, 0.664730), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-95.234283, 48.273716, 59.989445),Vector3f(-0.589577, -0.282767, 0.756599), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-88.277267, 75.138306, 110.061523),Vector3f(0.634420, -0.409222, 0.655781), NORMAL));



    // water

m_cps.push_back(CameraCP(Vector3f(-72.180923, 41.388844, 126.699829),Vector3f(0.634420, -0.409222, 0.655781), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-57.339947, 28.738895, 167.085114),Vector3f(0.929269, -0.364681, 0.058880), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-55.263920, 28.738895, 203.188248),Vector3f(0.879082, -0.342193, -0.331842), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-21.233406, 28.738895, 226.278442),Vector3f(0.310010, -0.332060, -0.890859), NORMAL));
m_cps.push_back(CameraCP(Vector3f(15.141066, 28.738895, 230.679047),Vector3f(-0.121480, -0.332101, -0.935389), NORMAL));
m_cps.push_back(CameraCP(Vector3f(24.007334, 12.502090, 216.324890),Vector3f(0.015982, -0.789055, -0.614114), NORMAL));
m_cps.push_back(CameraCP(Vector3f(53.880318, 29.843380, 203.784988),Vector3f(-0.842726, -0.458875, -0.281507), NORMAL));
m_cps.push_back(CameraCP(Vector3f(61.479885, 29.843380, 174.861633),Vector3f(-0.874485, -0.457324, -0.161650), NORMAL));
m_cps.push_back(CameraCP(Vector3f(56.494949, 29.843380, 132.433334),Vector3f(-0.760465, -0.419961, 0.495305), NORMAL));
m_cps.push_back(CameraCP(Vector3f(26.350430, 29.843380, 115.650993),Vector3f(-0.249049, -0.466502, 0.848734), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-6.079349, 29.843380, 114.270706),Vector3f(0.161036, -0.444970, 0.880948), NORMAL));
m_cps.push_back(CameraCP(Vector3f(-37.892269, 22.786026, 139.972122),Vector3f(0.414134, -0.669222, 0.616956), NORMAL));



    // road

m_cps.push_back(CameraCP(Vector3f(-0.811351, 31.887959, 150.194229),Vector3f(0.980844, -0.076736, 0.179043), NORMAL));
m_cps.push_back(CameraCP(Vector3f(53.333313, 60.782364, 203.527542),Vector3f(0.907942, -0.336887, -0.249295), NORMAL));
m_cps.push_back(CameraCP(Vector3f(93.240089, 79.123657, 223.222916),Vector3f(0.766376, -0.438159, -0.469771), NORMAL));
m_cps.push_back(CameraCP(Vector3f(128.863708, 79.123657, 251.246811),Vector3f(0.386221, -0.422422, -0.819996), NORMAL));
m_cps.push_back(CameraCP(Vector3f(194.444305, 14.558177, 210.983917),Vector3f(0.005958, -0.503726, -0.863843), NORMAL));
m_cps.push_back(CameraCP(Vector3f(191.527740, 16.485430, 146.665451),Vector3f(-0.153991, 0.107102, -0.982251), NORMAL));
m_cps.push_back(CameraCP(Vector3f(180.250229, 22.748009, 98.053757),Vector3f(-0.438436, 0.117558, -0.891041), NORMAL));
m_cps.push_back(CameraCP(Vector3f(128.215790, 48.206280, 53.831795),Vector3f(-0.951931, -0.286213, -0.109130), NORMAL));



    // grass

    m_cps.push_back(CameraCP(Vector3f(55.804581, 14.754387, 46.565681),Vector3f(-0.988114, -0.127619, -0.085699), SLOW));
    m_cps.push_back(CameraCP(Vector3f(11.049775, 4.955959, 42.684090),Vector3f(-0.988114, -0.127619, -0.085699), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-9.777554, 3.599994, 41.233894),Vector3f(-0.995610, -0.063349, -0.068899), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-30.054684, 4.556638, 39.900890),Vector3f(-0.996684, 0.048290, -0.065484), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-82.403900, 33.241173, 45.793343),Vector3f(0.872747, -0.468854, 0.135975), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-71.049828, 33.241173, 74.416710),Vector3f(0.630088, -0.496310, -0.597215), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-46.421425, 33.241173, 87.309029),Vector3f(0.206112, -0.505363, -0.837930), SLOW));
    m_cps.push_back(CameraCP(Vector3f(-17.885048, 33.241173, 84.365913),Vector3f(-0.209766, -0.583483, -0.784567), SLOW));
    // WIND PUFF.
    m_cps.push_back(CameraCP(Vector3f(2.374593, 33.241173, 78.393082),Vector3f(-0.314151, -0.518026, -0.795587), SLOW));
    m_cps.push_back(CameraCP(Vector3f(26.869501, 33.241173, 61.025742),Vector3f(-0.615767, -0.591554, -0.520477), SLOW));



m_cps.push_back(CameraCP(Vector3f(17.974758, 25.180792, 44.513790),Vector3f(-0.302026, -0.254402, -0.918728), 0.4f));
m_cps.push_back(CameraCP(Vector3f(10.410393, 18.809195, 21.503857),Vector3f(-0.302026, -0.254402, -0.918728), 0.4f));
m_cps.push_back(CameraCP(Vector3f(3.177092, 11.501963, 7.045398),Vector3f(-0.407704, -0.411871, -0.814948), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.844770, 10.689586, 3.126313),Vector3f(0.760087, -0.330314, -0.559607), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.602687, 9.219273, -1.230264),Vector3f(0.982004, -0.182273, -0.049445), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.631210, 9.219273, -1.796738),Vector3f(0.982004, -0.182273, -0.049445), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.663671, 9.219273, -2.441433),Vector3f(0.982004, -0.182273, -0.049445), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.717020, 9.219273, -3.500958),Vector3f(0.982004, -0.182273, -0.049445), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.667086, 9.219273, -4.835662),Vector3f(0.464011, -0.139819, 0.874726), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-2.381591, 9.219273, -5.337639),Vector3f(0.250137, -0.260662, 0.932463), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-1.454742, 9.219273, -5.238940),Vector3f(-0.271567, -0.320555, 0.907467), 0.4f));
m_cps.push_back(CameraCP(Vector3f(0.108914, 9.019445, -4.591534),Vector3f(-0.528894, -0.264116, 0.806544), 0.4f));
m_cps.push_back(CameraCP(Vector3f(0.828720, 9.019445, -3.091085),Vector3f(-0.985651, -0.107183, 0.130399), 0.4f));
m_cps.push_back(CameraCP(Vector3f(0.917897, 9.019445, -2.417017),Vector3f(-0.985651, -0.107183, 0.130399), 0.4f));
m_cps.push_back(CameraCP(Vector3f(1.034228, 9.019445, -1.537702),Vector3f(-0.985651, -0.107183, 0.130399), 0.4f));
m_cps.push_back(CameraCP(Vector3f(1.126383, 9.019445, -0.841125),Vector3f(-0.985651, -0.107183, 0.130399), 0.4f));


// ao

m_cps.push_back(CameraCP(Vector3f(33.731110, 38.669220, -33.211575),Vector3f(0.512008, -0.486795, -0.707728), 0.2f));
m_cps.push_back(CameraCP(Vector3f(39.536346, 43.263008, -42.080627),Vector3f(0.391015, -0.534796, -0.749066), 0.2f));
m_cps.push_back(CameraCP(Vector3f(39.788837, 54.742615, -94.366524),Vector3f(-0.426733, -0.497595, -0.755181), 0.2f));
m_cps.push_back(CameraCP(Vector3f(7.259646, 74.768410, -105.484940),Vector3f(-0.942659, -0.320555, -0.092946), 0.2f));
m_cps.push_back(CameraCP(Vector3f(-25.384199, 63.000370, -128.800995),Vector3f(-0.942659, -0.320555, -0.092946), 0.2f));
m_cps.push_back(CameraCP(Vector3f(-73.040070, 48.349751, -119.027237),Vector3f(-0.746228, -0.221749, 0.627672), 0.2f));
m_cps.push_back(CameraCP(Vector3f(-80.756493, 43.240578, -82.077065),Vector3f(0.523720, -0.532117, 0.665259), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-42.949142, 23.981281, -43.050602),Vector3f(0.666973, -0.288362, 0.687019), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-24.097486, 15.830881, -23.632339),Vector3f(0.666973, -0.288362, 0.687019), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.542187, 9.864868, -3.347146),Vector3f(0.653034, -0.217470, 0.725433), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.693598, 9.265701, -2.659110),Vector3f(0.681054, -0.480876, 0.552199), 0.24));


m_cps.push_back(CameraCP(Vector3f(-4.328483, 10.224816, -4.548865),Vector3f(0.582884, -0.500601, 0.640035), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-2.002702, 10.224816, -4.940857),Vector3f(0.006355, -0.519534, 0.854426), 0.4f));
m_cps.push_back(CameraCP(Vector3f(0.394761, 10.224816, -2.907647),Vector3f(-0.743865, -0.560619, 0.363830), 0.4f));
m_cps.push_back(CameraCP(Vector3f(0.381110, 10.224816, 0.113322),Vector3f(-0.689337, -0.605916, -0.397089), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-1.295857, 10.224816, 1.613147),Vector3f(-0.283871, -0.604551, -0.744268), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-2.416543, 10.224816, 1.691109),Vector3f(0.036477, -0.634645, -0.771943), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.565540, 10.224816, 0.955411),Vector3f(0.557142, -0.617449, -0.555292), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.283578, 10.224816, -0.144000),Vector3f(0.704258, -0.603652, -0.373664), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-4.585760, 10.224816, -1.403400),Vector3f(0.779808, -0.610586, -0.138146), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));
m_cps.push_back(CameraCP(Vector3f(-3.794848, 9.496401, -1.966248),Vector3f(0.742719, -0.655161, -0.138323), 0.4f));


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
    LOG_I("m_cps.push_back(CameraCP(%s,%s, 0.4f));",
	  (string("Vector3f") + string(m_position)).c_str(),
	  (string("Vector3f") + string(m_viewDir)).c_str() );
}
/*PrintState:CameraCP((-52.844795, 14.650712, 35.442703),(0.933511, -0.318891, 0.163909))
INFO: /Users/eric/tuhu/ewa/camera.cpp:173:PrintState:CameraCP((-28.865242, 6.459203, 39.653118),(0.933511, -0.318891, 0.163909))
 */

template <class T>
T Lerp(const T& cp1, const T& cp2, float t ) {
    /*
    return Vector3f(
	Lerp(cp1.x, cp2.x, t),
	Lerp(cp1.y, cp2.y, t),
	Lerp(cp1.z, cp2.z, t)
	);
    */

    return cp1 * (1.0 -t) + cp2 * (t);
}

template <class T>
T CatmullRomSpline(const T &P1,const T &P2,const T &P3,const T &P4,float t)
{
    float t2 = t * t;
    float t3 = t * t2;

    return ((P2 * 2.0f) + (-P1 + P3) * t + (P1 * 2.0f - P2 * 5.0f + P3 * 4.0f - P4) * t2 + (-P1 + P2 * 3.0f - P3 * 3.0f + P4) * t3) * 0.5f;
}


void Camera::Interpolate(const float delta) {

    static int i_cp = 0;


    int size = m_cps.size();

#define WRAP(I) ((I) >= size ? (I) % size : (  (I) < 0 ? (size + (I) )  : (I) ) )
	int ip1 = WRAP(i_cp-1);
	int ip2 = WRAP(i_cp+0);
	int ip3 = WRAP(i_cp+1);
	int ip4 = WRAP(i_cp+2);
#undef WRAP

    CameraCP cp1 = m_cps[ip1];
    CameraCP cp2 = m_cps[ip2];
    CameraCP cp3 = m_cps[ip3];
    CameraCP cp4 = m_cps[ip4];

    float velocity = CatmullRomSpline(cp1.m_velocity, cp2.m_velocity, cp3.m_velocity, cp4.m_velocity, m_t);

    m_t += delta*velocity;


    if(m_t <= 1.0) {



	m_position = CatmullRomSpline(cp1.m_position, cp2.m_position, cp3.m_position, cp4.m_position, m_t);
	m_viewDir = CatmullRomSpline(cp1.m_viewDir, cp2.m_viewDir, cp3.m_viewDir, cp4.m_viewDir, m_t);

//	LOG_I("lerp: %s", string(m_position).c_str() );

	ComputeViewMatrix();

    } else {
	i_cp++;
	m_t -= 1.0f;
    }
}
