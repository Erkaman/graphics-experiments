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

/*
   InitCamera(
	CameraCP(Vector3f(56.048920, 54.988255, 18.435148),Vector3f(-0.824103, -0.542272, 0.163692), FAST_SPEED));
*/

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



// move through torches section
m_cps.push_back(CameraCP(Vector3f(-83.791084, 27.635519, 11.390510),Vector3f(0.174189, -0.215369, -0.960872), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-78.415321, 21.640173, -18.104256),Vector3f(0.179770, -0.148452, -0.972443), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-75.720940, 19.303089, -35.560757),Vector3f(0.151225, -0.131170, -0.979758), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-74.284431, 16.447838, -55.730537),Vector3f(0.070343, -0.139816, -0.987676), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-73.485245, 13.994538, -81.491997),Vector3f(0.030868, -0.094757, -0.995022), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-73.528694, 11.000702, -101.201118),Vector3f(-0.002179, -0.150178, -0.988657), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-73.562851, 8.645479, -116.706093),Vector3f(-0.002179, -0.150178, -0.988657), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-83.372009, 11.992295, -128.592926),Vector3f(-0.621983, 0.212216, -0.753725), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-93.218544, 19.805672, -138.397461),Vector3f(-0.617666, 0.490128, -0.615032), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-100.889664, 27.331621, -145.954956),Vector3f(-0.583896, 0.572847, -0.575249), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-102.198639, 28.032572, -146.878693),Vector3f(-0.664561, 0.476379, -0.575693), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-111.339821, 34.585278, -154.797485),Vector3f(-0.753390, -0.020084, -0.657267), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-120.589920, 34.338696, -162.867386),Vector3f(-0.786647, -0.140039, -0.601312), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-135.480789, 31.687828, -174.250015),Vector3f(-0.680072, -0.526213, -0.510493), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-152.666611, 18.390120, -187.150436),Vector3f(-0.680072, -0.526213, -0.510493), NORMAL_SPEED));


// move to water
m_cps.push_back(CameraCP(Vector3f(-153.844101, 18.110344, -187.655701),Vector3f(-0.905925, -0.176381, -0.384954), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-173.763245, 14.232144, -196.119934),Vector3f(-0.905925, -0.176381, -0.384954), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-193.375809, 12.030952, -186.620590),Vector3f(-0.895434, -0.100498, 0.433703), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-203.200775, 11.358441, -165.808304),Vector3f(-0.426714, -0.029208, 0.903915), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-205.793884, 12.343373, -137.767044),Vector3f(0.076118, 0.203083, 0.976198), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-207.681702, 13.317909, -121.134476),Vector3f(-0.116964, -0.129973, 0.984595), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-210.541763, 10.139715, -97.058456),Vector3f(-0.116964, -0.129973, 0.984595), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-213.121628, 7.272889, -75.341148),Vector3f(-0.116964, -0.129973, 0.984595), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-212.937378, 6.496160, -54.837864),Vector3f(0.008979, -0.037854, 0.999243), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-205.364120, 12.938257, -30.529963),Vector3f(0.288366, 0.245295, 0.925568), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-201.334351, 17.238787, -3.054873),Vector3f(0.143407, 0.153043, 0.977759), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-194.877014, 23.419943, 19.253941),Vector3f(0.268686, 0.257194, 0.928256), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-184.549225, 25.716290, 43.933296),Vector3f(0.384625, 0.085520, 0.919103), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-160.683182, 27.954170, 68.903587),Vector3f(0.689495, 0.064653, 0.721399), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-134.099045, 21.762417, 97.611046),Vector3f(0.671100, -0.156307, 0.724702), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-111.857727, 16.582153, 121.628838),Vector3f(0.671100, -0.156307, 0.724702), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(-89.580360, 18.980896, 141.418930),Vector3f(0.745199, 0.080240, 0.661997), FAST_SPEED));




    // see water reflection.
m_cps.push_back(CameraCP(Vector3f(-71.283531, 20.951025, 157.672928),Vector3f(0.773636, 0.005279, 0.633608), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-58.256042, 20.667902, 169.517471),Vector3f(0.640385, -0.080067, 0.763869), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-48.717014, 19.837286, 186.364990),Vector3f(0.398591, -0.023014, 0.916840), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-42.017071, 27.436459, 201.776199),Vector3f(0.398591, -0.023014, 0.916840), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-34.182987, 20.052683, 219.883270),Vector3f(0.371890, -0.350513, 0.859557), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-34.182987, 20.052683, 219.883270),Vector3f(0.145276, -0.169991, -0.974678), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-26.447483, 22.775572, 250.466171),Vector3f(0.145276, -0.169991, -0.974678), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-12.091878, 22.775572, 252.605865),Vector3f(0.145276, -0.169991, -0.974678), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-0.689209, 22.775572, 251.833359),Vector3f(-0.067146, -0.115014, -0.991092), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(16.115831, 22.775572, 250.694824),Vector3f(-0.067146, -0.115014, -0.991092), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(35.784164, 22.775572, 249.362289),Vector3f(-0.067146, -0.115014, -0.991092), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(51.721867, 22.775572, 248.282532),Vector3f(-0.067146, -0.115014, -0.991092), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(66.629799, 22.775572, 247.272537),Vector3f(-0.067146, -0.115014, -0.991092), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(68.073105, 37.555347, 233.560120),Vector3f(0.104164, 0.098817, -0.989639), NORMAL_SPEED));




m_cps.push_back(CameraCP(Vector3f(79.392395, 28.633308, 215.395508),Vector3f(0.488153, -0.384770, -0.783363), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(93.992508, 20.866411, 202.341827),Vector3f(0.692978, -0.368647, -0.619581), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(119.135315, 6.038726, 189.251144),Vector3f(0.851019, -0.281854, -0.443086), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(132.507401, 4.021211, 182.950134),Vector3f(0.896293, -0.135228, -0.422341), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(132.507401, 74.605087, 182.950134),Vector3f(0.580326, -0.770198, -0.264608), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(119.177689, 97.468140, 189.027985),Vector3f(0.580326, -0.770198, -0.264608), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(159.662109, 97.468140, 228.617798),Vector3f(0.297748, -0.715600, -0.631872), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(216.460648, 97.468140, 232.834442),Vector3f(-0.294927, -0.679422, -0.671865), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(269.112122, 97.468140, 167.259109),Vector3f(-0.702888, -0.708298, -0.065291), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(262.893585, 97.468140, 103.539497),Vector3f(-0.750131, -0.646685, 0.138212), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(242.526840, 83.293686, 86.855072),Vector3f(-0.613020, -0.423278, -0.667115), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(222.116455, 66.782822, 48.952320),Vector3f(-0.288946, -0.301616, -0.908591), NORMAL_SPEED));



    // fly to grass field.

m_cps.push_back(CameraCP(Vector3f(211.910217, 59.021523, 26.535688),Vector3f(-0.550342, -0.299962, -0.779196), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(189.569504, 48.585934, 2.835130),Vector3f(-0.803044, -0.299963, -0.514920), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(160.577942, 39.147469, -8.813809),Vector3f(-0.956130, -0.242941, -0.163695), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(129.459106, 29.796947, -4.881447),Vector3f(-0.957017, -0.220501, 0.188407), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(103.331009, 19.269592, 5.976056),Vector3f(-0.957017, -0.220501, 0.188407), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(74.020538, 13.885333, 14.733985),Vector3f(-0.972064, -0.122536, 0.200192), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(56.048920, 11.619878, 18.435148),Vector3f(-0.972064, -0.122536, 0.200192), FAST_SPEED));
m_cps.push_back(CameraCP(Vector3f(56.048920, 54.988255, 18.435148),Vector3f(-0.824103, -0.542272, 0.163692), FAST_SPEED));



m_cps.push_back(CameraCP(Vector3f(39.620399, 44.178047, 21.698355),Vector3f(-0.824103, -0.542272, 0.163692), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(18.733742, 12.608284, 25.847078),Vector3f(-0.926626, -0.327852, 0.184056), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(5.674056, 7.987608, 28.441132),Vector3f(-0.987288, -0.127958, 0.094286), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-5.620661, 6.523746, 29.519781),Vector3f(-0.987288, -0.127958, 0.094286), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-14.257308, 8.668011, 30.124790),Vector3f(-0.984975, -0.143525, 0.096051), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-25.122328, 7.041368, 31.184303),Vector3f(-0.980469, -0.172820, 0.093879), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-45.267605, 7.930465, 33.113194),Vector3f(-0.980469, -0.172820, 0.093879), NORMAL_SPEED));
m_cps.push_back(CameraCP(Vector3f(-62.882065, 13.570691, 34.799759),Vector3f(-0.980469, -0.172820, 0.093879), NORMAL_SPEED));

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
