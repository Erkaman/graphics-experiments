#include "snow_effect.hpp"

#include "ewa/gl/texture2d.hpp"
#include "ewa/log.hpp"

const Vector3f SNOW_HEIGHT = Vector3f(0,1.0,0) ;

SnowEffect::SnowEffect( const Vector3f& CameraPos) {
    SetMinVelocity(Vector3f(0,-0.6f,0));
    SetMaxVelocity(Vector3f(0,-0.7f,0));
    SetMaxParticles(2500);
    SetEmitRate(0.01f);
    SetParticleLifetime(2.5f);
    SetSize(0.04f);
    SetEmitPosition(CameraPos + SNOW_HEIGHT); // emit position will be following player, so not point in setting.
    SetEmitPositionVariance(Vector3f(4,0,4) );
    SetWarmupFrames(0);
    SetEmitCount(10);
    SetColor(Color(1,1,1,1));

    Texture* texture = Texture2D::Load("img/snow.png");
    if(!texture) {
	PrintErrorExit();
    }

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);
}

void SnowEffect::Render(const Matrix4f& VP, const Vector3f& CameraPos) {
    SetBaseEmitPosition(CameraPos + SNOW_HEIGHT );
    ParticleSystem::Render(VP, CameraPos);
}