#include "particle_system.hpp"
#include "gbuffer.hpp"

#include "ewa/math/vector3f.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include "ewa/common.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/query.hpp"

#include "ewa/random_texture.hpp"
#include "ewa/config.hpp"

using std::vector;

ParticleSystem::~ParticleSystem()
{
    MY_DELETE(m_texture);
    MY_DELETE(m_randomTexture);

    MY_DELETE(m_particleBillboardShader);
}

ParticleSystem::ParticleSystem(){
    m_time = 0;
    m_texture = NULL;

    SetMaxParticles(1000);
    SetWarmupFrames(0);
    SetEmitCount(1);
    SetColor(Color(1,1,1,1));
    SetBlendingMode(ALPHA_BLENDING_MODE);
    SetParticleLifetimeVariance(0.0f);
    SetStartSizeVariance(0);
    SetEndSizeVariance(0);
}

void ParticleSystem::Init(){

    m_randomTexture = new RandomTexture(1000, 2);

    m_particleBillboardShader = ShaderProgram::Load("shader/part_billboard");

    m_particleBillboardShader->Bind();
}

void ParticleSystem::Update(float delta){
    UpdateParticles(delta);
}

void ParticleSystem::UpdateParticles(float delta){
    m_time += delta;
}

void ParticleSystem::Render(Gbuffer* gbuffer, const Matrix4f& VP, const Vector3f& CameraPos, int windowWidth, int windowHeight, const Vector3f& emitPosition){







    const int NUM_PARTICLES = 150;

    m_particleBillboardShader->SetUniform("numParticles", (float)NUM_PARTICLES );
    m_particleBillboardShader->SetUniform("emitPosition", emitPosition );
    m_particleBillboardShader->SetUniform("time", m_time + 10000);


    GL_C(glDrawArrays(GL_POINTS, 0, NUM_PARTICLES));







}


void ParticleSystem::RenderSetup(Gbuffer* gbuffer, const Matrix4f& VP, const Vector3f& CameraPos, int windowWidth, int windowHeight){

    m_particleBillboardShader->Bind();
    m_particleBillboardShader->SetUniform("gCameraPos", CameraPos);
    m_particleBillboardShader->SetUniform("gVP", VP);


    m_particleBillboardShader->SetUniform("minVelocity", m_minVelocity);
    m_particleBillboardShader->SetUniform("maxVelocity", m_maxVelocity);

    m_particleBillboardShader->SetUniform("windowWidth", (float)windowWidth);
    m_particleBillboardShader->SetUniform("windowHeight", (float)windowHeight);

    m_particleBillboardShader->SetUniform("baseParticleLifetime", m_particleLifetime );
    m_particleBillboardShader->SetUniform("particleLifetimeVariance", m_particleLifetimeVariance );

    m_particleBillboardShader->SetUniform("baseStartSize", m_startSize );
    m_particleBillboardShader->SetUniform("startSizeVariance", m_startSizeVariance );

    m_particleBillboardShader->SetUniform("baseEndSize", m_endSize );
    m_particleBillboardShader->SetUniform("endSizeVariance", m_endSizeVariance );

    m_particleBillboardShader->SetUniform("emitPositionVariance", m_emitPositionVariance );

    m_particleBillboardShader->SetUniform("emitRate", m_emitRate);
    m_particleBillboardShader->SetUniform("emitCount", m_emitCount );

    Config& config = Config::GetInstance();

    m_particleBillboardShader->SetUniform("znear", config.GetZNear() );
    m_particleBillboardShader->SetUniform("zfar", config.GetZFar() );


    m_particleBillboardShader->SetUniform("startColor", m_startColor );
    m_particleBillboardShader->SetUniform("endColor", m_endColor );

    m_particleBillboardShader->SetUniform("randomTexture", 1);
    Texture::SetActiveTextureUnit(1);
    m_randomTexture->Bind();


    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(m_sfactor, m_dfactor));

    m_particleBillboardShader->SetUniform("gColorMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_texture->Bind();

    m_particleBillboardShader->SetUniform("depthTexture", 7);
    Texture::SetActiveTextureUnit(7);
    gbuffer->GetDepthTexture()->Bind();


    SetDepthTest(false);

}

void ParticleSystem::RenderUnsetup(Gbuffer* gbuffer, const Matrix4f& VP, const Vector3f& CameraPos, int windowWidth, int windowHeight) {
    SetDepthTest(true);

    GL_C(glDisable(GL_BLEND));

    m_particleBillboardShader->Unbind();

    gbuffer->GetDepthTexture()->Unbind();
    m_texture->Unbind();

}

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

void ParticleSystem::SetBaseParticleLifetime(float particleLifetime) {
    m_particleLifetime = particleLifetime;
}

void ParticleSystem::SetBaseEmitPosition(const Vector3f& emitPosition) {
	m_emitPosition = emitPosition;
}

void ParticleSystem::SetEmitPosition(const Vector3f& emitPosition) {
	SetBaseEmitPosition(emitPosition);
	SetEmitPositionVariance(Vector3f(0.0f, 0.0f, 0.0f));
}

void ParticleSystem::SetEmitPositionVariance(const Vector3f& emitPositionVariance) {
    m_emitPositionVariance = emitPositionVariance;
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


void ParticleSystem::SetBaseStartSize(const float startSize) {
    m_startSize = startSize;
}

void ParticleSystem::SetBaseEndSize(const float endSize) {
    m_endSize = endSize;
}

void ParticleSystem::SetSize(const float size) {
    SetBaseStartSize(size);
    SetBaseEndSize(size);
    SetStartSizeVariance(0.0f);
    SetEndSizeVariance(0.0f);
}

void ParticleSystem::SetStartColor(const Color& startColor) {
    m_startColor = startColor;
}

void ParticleSystem::SetEndColor(const Color& endColor) {
    m_endColor = endColor;
}

void ParticleSystem::SetColor(const Color& color) {
    SetStartColor(color);
    SetEndColor(color);
}

void ParticleSystem::SetBlendingMode(const ColorBlendingMode blendingMode) {
    if(blendingMode == ALPHA_BLENDING_MODE) {
	m_sfactor = GL_SRC_ALPHA;
	m_dfactor = GL_ONE_MINUS_SRC_ALPHA;
    } else if(blendingMode == ADDITIVE_BLENDING_MODE) {
	m_sfactor = GL_SRC_ALPHA;
	m_dfactor = GL_ONE;
    } else {
	assert(false);
    }
}

void ParticleSystem::SetParticleLifetimeVariance(const float particleLifetimeVariance) {
    m_particleLifetimeVariance = particleLifetimeVariance;
}

void ParticleSystem::SetParticleLifetime(float particleLifetime) {
    SetBaseParticleLifetime(particleLifetime);
    SetParticleLifetimeVariance(0);
}

void ParticleSystem::SetStartSizeVariance(const float startSizeVariance) {
    m_startSizeVariance = startSizeVariance;
}

void ParticleSystem::SetEndSizeVariance(const float endSizeVariance) {
    m_endSizeVariance = endSizeVariance;
}

void ParticleSystem::SetStartSize(const float startSize) {
    SetBaseStartSize(startSize);
    SetStartSizeVariance(0.0);
}

void ParticleSystem::SetEndSize(const float endSize) {
    SetBaseEndSize(endSize);
    SetEndSizeVariance(0.0);
}
