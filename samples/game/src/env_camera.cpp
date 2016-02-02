#include "env_camera.hpp"

#include "ewa/config.hpp"

EnvCamera::EnvCamera(const Vector3f& position, int i) {

    m_i = i;

/*
    if(target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y) {
	m_forward = Vector3f(0,1,0);
	m_right = Vector3f(1,0,0);
	m_up = Vector3f(0,0,1);
    }

    m_forward.Normalize();
    m_right.Normalize();
    m_up.Normalize();
*/

    Config& config = Config::GetInstance();


    m_projectionMatrix =
	Matrix4f::CreatePerspective (90.0f, 1.0f, config.GetZNear(), 300.0f);

//    Update(0);
}
/*
    if(!cubeMap->LoadCubemapFace(leftFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_X)) return NULL;
    if(!cubeMap->LoadCubemapFace(rightFace, GL_TEXTURE_CUBE_MAP_POSITIVE_X))return NULL;
    if(!cubeMap->LoadCubemapFace(downFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y))return NULL;
    if(!cubeMap->LoadCubemapFace(upFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Y))return NULL;
    if(!cubeMap->LoadCubemapFace(backFace, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))return NULL;
    if(!cubeMap->LoadCubemapFace(frontFace, GL_TEXTURE_CUBE_MAP_POSITIVE_Z))return NULL;
*/


void EnvCamera::Update(const float delta) {

    Vector3f lookAt(0,0,0), up(0,0,0), right(0,0,0);
    switch(m_i) {
    case 0:  lookAt.x =-1;  up.y = 1;  right.z = 1;  break;  // +X
    case 1:  lookAt.x = 1;  up.y = 1;  right.z =-1;  break;	 // -X
    case 2:  lookAt.y =-1;  up.z = 1;  right.x = 1;  break;	 // +Y
    case 3:  lookAt.y = 1;  up.z =-1;  right.x = 1;  break;	 // -Y
    case 4:  lookAt.z = 1;  up.y = 1;  right.x =-1;  break;	 // +Z
    case 5:  lookAt.z =-1;  up.y = 1;  right.x =-1;  break;	 // -Z
    }


    Vector3f zaxis = lookAt; // forward vector.
    Vector3f xaxis = right; // right vector.
	Vector3f yaxis = up; // up vector.

    Matrix4f orientation(
	xaxis.x, xaxis.y, xaxis.z, 0,
	yaxis.x, yaxis.y, yaxis.z, 0 ,
	zaxis.x, zaxis.y, zaxis.z, 0 ,
	0,       0,       0,     1
	);

    Vector3f eyeNeg = -m_position;

//    LOG_I("pos: %s", std::string(m_position).c_str() );

    Matrix4f translation = Matrix4f::CreateTranslation(eyeNeg);

    m_viewMatrix = orientation * translation;



//    m_viewMatrix = Matrix4f::CreateLookAt(m_position, );

}


Vector3f EnvCamera::GetPosition() const {
    return m_position;
}


void EnvCamera::SetPosition(const Vector3f& position) {
    m_position = position;
}
