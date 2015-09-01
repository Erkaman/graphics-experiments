#include "smoke_effect.hpp"

#include "ewa/gl/texture2d.hpp"


SmokeEffect::SmokeEffect(const Vector3f& position) {

    constexpr float A = 0.04886f;
    SetMinVelocity(Vector3f(-A,A,-A));
    SetMaxVelocity(Vector3f(A,1.0f / 20.0f,A));
    SetMaxParticles(1000);
    SetEmitRate(0.01f);
    SetParticleLifetime(7.0f);

    SetStartSize(0.20f);
    SetEndSize(0.10f);
    SetStartSizeVariance(0.01f);
    SetEndSizeVariance(0.01f);

    SetEmitPosition(position); //
    SetEmitPositionVariance(Vector3f(0.0f,0.0f,0.0f));

    float C = 0.77f;

    SetStartColor(Color(C,C,C,1.0));
    SetEndColor(Color(C,C,C,0.0));


    Texture* texture = new Texture2D("img/smoke2.png" );

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);
}
