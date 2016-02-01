#include "env_camera.hpp"

#include "ewa/config.hpp"

EnvCamera::EnvCamera(GLenum target, const Vector3f& position) {

    if(target == GL_TEXTURE_CUBE_MAP_POSITIVE_Y) {
	m_forward = Vector3f(0,1,0);
	m_right = Vector3f(1,0,0);
	m_forward = Vector3f(1,0,0);
    }

    Config& config = Config::GetInstance();


    m_projectionMatrix =
	Matrix4f::CreatePerspective (90.0f, 1.0f, config.GetZNear(), 50.0f);

    Update(0);
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
        Vector3f zaxis = m_forward.Normalize(); // forward vector.
    Vector3f xaxis = m_right.Normalize(); // right vector.
    Vector3f yaxis = m_up.Normalize(); // up vector.

    Matrix4f orientation(
	xaxis.x, xaxis.y, xaxis.z, 0,
	yaxis.x, yaxis.y, yaxis.z, 0 ,
	zaxis.x, zaxis.y, zaxis.z, 0 ,
	0,       0,       0,     1
	);

    Vector3f eyeNeg = -m_position;

    Matrix4f translation = Matrix4f::CreateTranslation(eyeNeg);
    m_viewMatrix = orientation * translation;
}


Vector3f EnvCamera::GetPosition() const {
    return m_position;
}
