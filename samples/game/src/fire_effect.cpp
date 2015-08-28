#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float V = 0.1486f;
    SetVelocity(Vector3f(0,V,0));
    SetMaxParticles(1000);
    SetEmitRate(0.1f);
    SetEmitCount(2);
    SetParticleLifetime(2.5f);
    SetStartSize(0.20f);
    SetEndSize(0.00f);


    SetStartColor(Color(1,0.50,0,1));
    SetEndColor(Color(1,0.50,0,0));


//    SetStartColor(Color(1,1,1,1));
//    SetEndColor(Color(1,1,1,0));

    SetBlendingMode(ALPHA_BLENDING_MODE);


    SetEmitPosition(position);
    SetEmitRange(Vector3f(0.05,0,0.05));

    Texture* texture = new Texture2D("img/snow.png");

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);

}

//#
