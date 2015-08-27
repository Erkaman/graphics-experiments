#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float V = 0.1886f;
    SetVelocity(Vector3f(0,V,0));
    SetMaxParticles(1000);
    SetEmitRate(0.1f);
    SetEmitCount(1);
    SetParticleLifetime(3.0f);
    SetStartSize(0.09f);
    SetEndSize(0.00f);

    SetEmitPosition(position);
    SetEmitRange(Vector3f(0.05,0,0.05));

    Texture* texture = new Texture2D("img/fire.png");

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);
}
