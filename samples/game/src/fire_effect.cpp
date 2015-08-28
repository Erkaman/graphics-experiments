#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float V = 0.3086f;
    SetVelocity(Vector3f(0,V,0));
    SetMaxParticles(1000);
    SetEmitRate(0.01f);
    SetEmitCount(1);
    SetParticleLifetime(2.5f);
    SetStartSize(0.20f);
    SetEndSize(0.01f);


/*    SetStartColor(Color(1,0.50,0,1));
    SetEndColor(Color(1,0.50,0,0));
*/
    SetStartColor(Color(1,0.3,0,0.6));
    SetEndColor(Color(1,0.3,0,0));

    SetBlendingMode(ADDITIVE_BLENDING_MODE);

    SetEmitPosition(position);
    SetEmitRange(Vector3f(0.00,0,0.0));

    Texture* texture = new Texture2D("img/particle_post.png");

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    SetTexture(texture);

}

//#

/*
lifespan variance is important.

size variance makes it look more interesting it seems.

getting a nice texture is important though.

http://onebyonedesign.com/flash/particleeditor/
*/
