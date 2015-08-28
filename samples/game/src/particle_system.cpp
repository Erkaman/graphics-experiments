#include "particle_system.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/matrix4f.hpp"

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
    float size;
    Vector4f color;
};

void beforeLinkingHook(GLuint shaderProgram) {
    const GLchar* varyings[6];
    varyings[0] = "type1";
    varyings[1] = "position1";
    varyings[2] = "velocity1";
    varyings[3] = "age1";
    varyings[4] = "size1";
    varyings[5] = "color1";

    GL_C(glTransformFeedbackVaryings(shaderProgram, 6, varyings, GL_INTERLEAVED_ATTRIBS));
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

    MY_DELETE(m_particleUpdateShader);
    MY_DELETE(m_particleBillboardShader);

}


ParticleSystem::ParticleSystem(){
    m_currVB = 0;
    m_currTFB = 1;
    m_isFirst = true;
    m_time = 0;
    m_texture = NULL;

    SetMaxParticles(1000);
    SetWarmupFrames(0);
    SetEmitCount(1);
    SetColor(Vector4f(1,1,1,1));
}

void ParticleSystem::Init(){
    Particle* Particles  = new Particle[m_maxParticles];

    Particles[0].type = PARTICLE_TYPE_EMITTER;
    Particles[0].pos = m_emitPosition;
    Particles[0].vel = Vector3f(0.0f);
    Particles[0].lifetime = 0.0f;
    Particles[0].size = 0.0f;
    Particles[0].color = Vector4f(0,0,0,0);

    GL_C(glGenTransformFeedbacks(2, m_transformFeedback));

    for (unsigned int i = 0; i < 2 ; i++) {


	m_particleBuffer[i] = VBO::CreateInterleaved(
	    vector<GLuint>{1,3,3,1, 1, 4}, // type, pos, vel, lifetime, size, color
	    GL_DYNAMIC_DRAW
	);

        GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]));


	m_particleBuffer[i]->Bind();


	m_particleBuffer[i]->SetBufferData(sizeof(Particle) * m_maxParticles, Particles);
	m_particleBuffer[i]->Unbind();

        GL_C(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]->GetBuffer()));
    }

    LOG_I("linking update");


    m_particleUpdateShader = new ShaderProgram("shader/part_update", beforeLinkingHook);

    LOG_I("linked update");

    m_randomTexture = new RandomTexture(1000, 2);


    m_particleBillboardShader = new ShaderProgram("shader/part_billboard");

    LOG_I("linked billboard");

    m_particleBillboardShader->Bind();


    delete [] Particles;

    // finally, we warm up the particle system.
/*    if(m_warmupFrames > 0) {


	const float DELTA = 1.0 / 60.0f; // TODO, fix so that it handles frame rates other than 60.


	GL_C(glEnable(GL_RASTERIZER_DISCARD));

	for(int i = 0; i < m_warmupFrames; ++i)  {

	    UpdateParticles(DELTA);

	    m_currVB = m_currTFB;
	    m_currTFB = (m_currTFB + 1) % 2;
	}

	GL_C(glDisable(GL_RASTERIZER_DISCARD));
	}*/


}

void ParticleSystem::Update(float delta){
    GL_C(glEnable(GL_RASTERIZER_DISCARD));

//    LOG_I("time: %f", delta);

    UpdateParticles(delta);

    GL_C(glDisable(GL_RASTERIZER_DISCARD));
}


void ParticleSystem::UpdateParticles(float delta){
    m_time += delta;

        m_particleUpdateShader->Bind();
    m_particleUpdateShader->SetUniform("time", m_time);
    m_particleUpdateShader->SetUniform("deltaTime", delta);

    m_particleUpdateShader->SetUniform("minVelocity", m_minVelocity);
    m_particleUpdateShader->SetUniform("maxVelocity", m_maxVelocity);
    m_particleUpdateShader->SetUniform("emitRate", m_emitRate);
    m_particleUpdateShader->SetUniform("particleLifetime", m_particleLifetime );
    m_particleUpdateShader->SetUniform("emitPosition", m_emitPosition );
    m_particleUpdateShader->SetUniform("emitRange", m_emitRange );
    m_particleUpdateShader->SetUniform("emitCount", m_emitCount );
    m_particleUpdateShader->SetUniform("startSize", m_startSize );
    m_particleUpdateShader->SetUniform("endSize", m_endSize );
    m_particleUpdateShader->SetUniform("startColor", m_startColor );
    m_particleUpdateShader->SetUniform("endColor", m_endColor );

/*    LOG_I("start: %s",  tos(m_startColor).c_str() );
    LOG_I("end: %s",  tos(m_endColor).c_str() );
*/


/*    LOG_I("start size: %f", m_startSize);
    LOG_I("end size: %f", m_endSize);
    LOG_I("particle life time: %f", m_particleLifetime);
*/


    m_particleUpdateShader->SetUniform("randomTexture", 0);
    Texture::SetActiveTextureUnit(0);
    m_randomTexture->Bind();


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

}

void ParticleSystem::Render(const Matrix4f& VP, const Vector3f& CameraPos){
    GL_C(glDisable(GL_RASTERIZER_DISCARD));

    RenderParticles(VP, CameraPos);

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) % 2;
}


void ParticleSystem::RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos){


    m_particleBillboardShader->Bind();
    m_particleBillboardShader->SetUniform("gCameraPos", CameraPos);
    m_particleBillboardShader->SetUniform("gVP", VP);
//    m_particleBillboardShader->SetUniform("gBillboardSize", m_billboardSize);


    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_C(glDepthMask(GL_FALSE) );

    m_particleBillboardShader->SetUniform("gColorMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_texture->Bind();

    m_particleBuffer[m_currTFB]->Bind();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0);  // type
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)32);  // size
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36);  // color

    GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    GL_C(glDisable(GL_BLEND));

    GL_C(glDepthMask(GL_TRUE) );

    m_particleBillboardShader->Unbind();


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

void ParticleSystem::SetEmitRate(float emitRate) {
    m_emitRate = emitRate;
}

void ParticleSystem::SetParticleLifetime(float particleLifetime) {
    m_particleLifetime = particleLifetime;
}

void ParticleSystem::SetEmitPosition(const Vector3f& emitPosition) {
    m_emitPosition = emitPosition;
}

void ParticleSystem::SetEmitRange(const Vector3f& emitRange) {
    m_emitRange = emitRange;
}


void ParticleSystem::SetWarmupFrames(const int warmupFrames) {
    m_warmupFrames = warmupFrames;
}

void ParticleSystem::SetEmitCount(const int emitCount) {
    m_emitCount = emitCount;
}


void ParticleSystem::SetVelocity(const Vector3f& vel) {
    SetMinVelocity(vel);
    SetMaxVelocity(vel);
}


void ParticleSystem::SetStartSize(const float startSize) {
    m_startSize = startSize;
}

void ParticleSystem::SetEndSize(const float endSize) {
    m_endSize = endSize;
}

void ParticleSystem::SetSize(const float size) {
    SetStartSize(size);
    SetEndSize(size);
}

void ParticleSystem::SetStartColor(const Vector4f& startColor) {
    m_startColor = startColor;
}

void ParticleSystem::SetEndColor(const Vector4f& endColor) {
    m_endColor = endColor;
}

void ParticleSystem::SetColor(const Vector4f& color) {
    SetStartColor(color);
    SetEndColor(color);
}
