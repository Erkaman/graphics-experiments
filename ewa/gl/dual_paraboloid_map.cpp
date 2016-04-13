#include "dual_paraboloid_map.hpp"

#include "ewa/gl/color_fbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/math/vector4f.hpp"

#include "ewa/aabb.hpp"
#include "ewa/config.hpp"



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

void Paraboloid::SetParaboloidUniforms(
    ShaderProgram& shader,
    const Matrix4f& modelMatrix, const Matrix4f& viewMatrix,
    const Matrix4f& projectionMatrix, const Vector3f&
    cameraPosition, const Vector4f& lightDirection
    )const {

    shader.SetShaderUniforms(modelMatrix, viewMatrix, projectionMatrix);

    shader.SetUniform("viewSpaceLightDirection", Vector3f(viewMatrix * (lightDirection)  ) );
    shader.SetUniform("eyePos", cameraPosition );

    // TODO: uniform zfar and znear.
    Config& config = Config::GetInstance();

    shader.SetUniform("znear", config.GetZNear() );
    shader.SetUniform("zfar", config.GetZFar() );
    shader.SetUniform("carPos", 	this->m_position );
    shader.SetUniform("paraboloidDirection", this->m_direction.z );

}
