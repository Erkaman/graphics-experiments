#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"

#include "ewa/log.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float minV = 0.886f;
    constexpr float maxV = minV + 0.2f;

    constexpr float range = 1.305f;

    SetMinVelocity(Vector3f(-range,minV,-range) );
    SetMaxVelocity(Vector3f(+range,maxV,+range));

    SetBaseParticleLifetime(1.5f);
    SetParticleLifetimeVariance(1.5f);

    SetBaseStartSize(0.70f);
    SetBaseEndSize(0.38f);
    SetStartSizeVariance(0.22f);
    SetEndSizeVariance(0.16f);

    SetStartColor(Color(1.0f,0.2f,0.0f,0.6f));
    SetEndColor(Color(1.0f,0.2f,0.0f,0.0f));

    SetBlendingMode(ADDITIVE_BLENDING_MODE);

    SetEmitPosition(position);
    SetEmitPositionVariance(Vector3f(0.05f));

    Texture* texture = Texture2D::Load("img/particle_post2.png");

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

//#

/*
lifespan variance is important.

size variance makes it look more interesting it seems.

getting a nice texture is important though.

http://onebyonedesign.com/flash/particleeditor/

start size variance big,
end size variance small.
*/
