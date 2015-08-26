#include "particle_system.h"

#include "ewa/math/vector3f.hpp"
#include "ewa/common.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/query.hpp"

#include "ewa/random_texture.hpp"

#define PARTICLE_LIFETIME 10.0f

#define PARTICLE_TYPE_EMITTER 0.0f
#define PARTICLE_TYPE_PARTICLE 1.0f

using std::vector;

struct Particle
{
    float type;
    Vector3f pos;
    Vector3f vel;
    float lifetime;
};

void beforeLinkingHook(GLuint shaderProgram) {
    const GLchar* varyings[4];
    varyings[0] = "type1";
    varyings[1] = "position1";
    varyings[2] = "velocity1";
    varyings[3] = "age1";

    GL_C(glTransformFeedbackVaryings(shaderProgram, 4, varyings, GL_INTERLEAVED_ATTRIBS));
}

ParticleSystem::~ParticleSystem()
{
    MY_DELETE(m_texture);
    MY_DELETE(m_randomTexture);

    if (m_transformFeedback[0] != 0) {
        glDeleteTransformFeedbacks(2, m_transformFeedback);
    }

    MY_DELETE(m_particleBuffer[0]);
    MY_DELETE(m_particleBuffer[1]);
}


ParticleSystem::ParticleSystem(){
    m_currVB = 0;
    m_currTFB = 1;
    m_isFirst = true;
    m_time = 0;
    m_texture = NULL;
}

void ParticleSystem::Init(const Vector3f& Pos){
    Particle* Particles  = new Particle[m_maxParticles];

    Particles[0].type = PARTICLE_TYPE_EMITTER;
    Particles[0].pos = Pos;
    Particles[0].vel = Vector3f(0.0f);
    Particles[0].lifetime = 0.0f;

    GL_C(glGenTransformFeedbacks(2, m_transformFeedback));

    for (unsigned int i = 0; i < 2 ; i++) {


	m_particleBuffer[i] = VBO::CreateInterleaved(
	    vector<GLuint>{1,3,3,1}, // type, pos, vel, lifetime
	    GL_DYNAMIC_DRAW
	);

        GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]));


	m_particleBuffer[i]->Bind();


	m_particleBuffer[i]->SetBufferData(sizeof(Particle) * m_maxParticles, Particles);
	m_particleBuffer[i]->Unbind();

        GL_C(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]->GetBuffer()));
    }

    m_particleUpdateShader = new ShaderProgram("shader/part_update", beforeLinkingHook);


    m_randomTexture = new RandomTexture(1000, 2);


    m_particleBillboardShader = new ShaderProgram("shader/part_billboard");

    m_particleBillboardShader->Bind();


    delete [] Particles;

}

void ParticleSystem::Update(float delta){
    m_time += delta;

        m_particleUpdateShader->Bind();
    m_particleUpdateShader->SetUniform("time", m_time);
    m_particleUpdateShader->SetUniform("deltaTime", delta);

    m_particleUpdateShader->SetUniform("minVelocity", m_minVelocity);
    m_particleUpdateShader->SetUniform("maxVelocity", m_maxVelocity);
    m_particleUpdateShader->SetUniform("emitInterval", 0.1f);
    m_particleUpdateShader->SetUniform("particleLifetime", 10.0f);


    m_particleUpdateShader->SetUniform("randomTexture", 0);
    Texture::SetActiveTextureUnit(0);
    m_randomTexture->Bind();


    GL_C(glEnable(GL_RASTERIZER_DISCARD));

    m_particleBuffer[m_currVB]->Bind();

    GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]));

    m_particleBuffer[m_currVB]->EnableVertexAttribInterleavedWithBind();

    GL_C(glBeginTransformFeedback(GL_POINTS));

    if (m_isFirst) {
	m_particleBuffer[m_currVB]->DrawVertices(GL_POINTS, 1);
        m_isFirst = false;
    }
    else {
        GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]));
    }

    GL_C(glEndTransformFeedback());

    m_particleBuffer[m_currVB]->DisableVertexAttribInterleavedWithBind();

    m_particleUpdateShader->Unbind();

    GL_C(glDisable(GL_RASTERIZER_DISCARD));
}

void ParticleSystem::Render(const Matrix4f& VP, const Vector3f& CameraPos){

    GL_C(glDisable(GL_RASTERIZER_DISCARD));

    m_particleBillboardShader->Bind();
    m_particleBillboardShader->SetUniform("gCameraPos", CameraPos);
    m_particleBillboardShader->SetUniform("gVP", VP);
    m_particleBillboardShader->SetUniform("gBillboardSize", 0.05f);


    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_C(glDepthMask(GL_FALSE) );

    m_particleBillboardShader->SetUniform("gColorMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_texture->Bind();

    m_particleBuffer[m_currTFB]->Bind();

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

    GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]));

    glDisableVertexAttribArray(0);

    GL_C(glDisable(GL_BLEND));

    GL_C(glDepthMask(GL_TRUE) );

    m_particleBillboardShader->Unbind();

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) % 2;
}

/*
  3. figure out how to emit per second.

 */

void ParticleSystem::SetMinVelocity(const Vector3f& vel) {
    m_minVelocity = vel;
}

void ParticleSystem::SetMaxVelocity(const Vector3f& vel) {
    m_maxVelocity = vel;
}

void ParticleSystem::SetMaxParticles(size_t maxParticles) {
    m_maxParticles = maxParticles;
}

void ParticleSystem::SetTexture(Texture* texture) {
    m_texture = texture;
}
