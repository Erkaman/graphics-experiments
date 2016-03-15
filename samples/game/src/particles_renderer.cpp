#include "particles_renderer.hpp"

#include "fire_effect.hpp"
#include "ewa/geometry_object.hpp"
#include "ewa/view_frustum.hpp"
#include "ewa/camera.hpp"


ParticlesRenderer::ParticlesRenderer() {
    m_fire = new FireEffect(Vector3f(0,20,0));
    m_fire->Init();
}


void ParticlesRenderer::Update(ViewFrustum* cameraFrustum, const float delta) {

    m_fire->Update(delta);

    m_particles.clear();

    for(Vector3f particle : GeometryObject::GetTorches()) {



	if(cameraFrustum->IsSphereInFrustum(particle.x, particle.y, particle.z, 1)  ) {
	    m_particles.push_back(particle);
	}

//	break;

    }
}

void ParticlesRenderer::Render(Gbuffer* gbuffer, ICamera* camera, int framebufferWidth, int framebufferHeight) {
    m_fire->RenderSetup(gbuffer, camera->GetVp(), camera->GetPosition(), framebufferWidth, framebufferHeight );

    for(Vector3f pos : m_particles) {


	m_fire->Render(gbuffer, camera->GetVp(), camera->GetPosition(), framebufferWidth,
		       framebufferHeight,  pos);

    }

    m_fire->RenderUnsetup(gbuffer, camera->GetVp(), camera->GetPosition(), framebufferWidth, framebufferHeight );

}
