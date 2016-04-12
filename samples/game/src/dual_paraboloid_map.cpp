#include "dual_paraboloid_map.hpp"

#include "ewa/gl/color_fbo.hpp"
#include "ewa/aabb.hpp"



DualParaboloidMap::DualParaboloidMap() {

    m_size = 512*2;

    for(int i = 0; i < 2; ++i) {
	m_paraboloids[i].m_fbo = new ColorFBO();
	m_paraboloids[i].m_fbo->Init(0, m_size, m_size);
	m_paraboloids[i].m_direction = i == 0 ? Vector3f(0,0,1) : Vector3f(0,0,-1);
	m_paraboloids[i].m_i = i;

    }
}


void DualParaboloidMap::Update(const Vector3f& carPosition) {

    for(int i = 0; i < 2; ++i) {

	Vector3f forward = m_paraboloids[i].m_direction	;

	m_paraboloids[i].m_viewMatrix = Matrix4f::CreateLookAt(
			carPosition,
			(carPosition + forward),
			Vector3f(0.0,1.0, 0.0) );
	m_paraboloids[i].m_position = carPosition;

    }
}

Paraboloid DualParaboloidMap::GetParaboloid(int i) {
    return m_paraboloids[i];
}


size_t DualParaboloidMap::GetSize() {
    return m_size;
}

bool Paraboloid::InFrustum(const Vector3f& position)const {
    return -1.0 * m_direction.z * (position - m_position ).z > 0.0f;
}

bool Paraboloid::InFrustum(const AABB& aabb) const{
    // if at least one point is in the frustum, it is in the frustum.
    return InFrustum(aabb.min) || InFrustum(aabb.max);
}
