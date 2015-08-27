#include "smoke_effect.hpp"

#include "ewa/gl/texture2d.hpp"


SmokeEffect::SmokeEffect(const Vector3f& position) {

    constexpr float A = 0.02886f;
    SetMinVelocity(Vector3f(-A,A,-A));
    SetMaxVelocity(Vector3f(A,1.0f / 20.0f,A));
    SetMaxParticles(1000);
    SetEmitRate(0.1f);
    SetParticleLifetime(10.0f);
    SetSize(0.09f);
    SetEmitPosition(position); //
    SetEmitRange(Vector3f(0,0,0));


    Texture* texture = new Texture2D("img/smoke2.png" );

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);

}
