#include "particles_renderer.hpp"
#include "resource_manager.hpp"

#include "fire_effect.hpp"
#include "ewa/geometry_object.hpp"
#include "ewa/view_frustum.hpp"
#include "ewa/camera.hpp"

using std::vector;
using std::string;

ParticlesRenderer::ParticlesRenderer() {
    m_fire = new FireEffect(Vector3f(0,20,0));
    m_fire->Init();

    vector<string> defines;

    m_compositeShader =
		ResourceManager::LoadShader(string("shader/composite") + "_vs.glsl",
					    string("shader/composite") + "_fs.glsl", defines);
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



//    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    m_compositeShader->Bind();


    GL_C(glEnable(GL_BLEND));
    GL_C(glBlendFunc(GL_ONE, GL_ONE));


    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    GL_C(glDisable(GL_BLEND)); // all the billboards use alpha blending.


    m_compositeShader->Unbind();
    */
}
