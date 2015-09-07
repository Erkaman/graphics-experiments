#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float V = 0.3086f;

    constexpr float range = 0.03f;

    SetMinVelocity(Vector3f(-range,V,-range) );
    SetMaxVelocity(Vector3f(+range,V,+range));

    SetMaxParticles(1000);
    SetEmitRate(0.01f);
    SetEmitCount(1);
    SetBaseParticleLifetime(1.5f);
    SetParticleLifetimeVariance(0.9f);

    SetBaseStartSize(0.20f);
    SetBaseEndSize(0.09f);
    SetStartSizeVariance(0.06f);
    SetEndSizeVariance(0.03f);

    SetStartColor(Color(1.0f,0.3f,0.0f,0.6f));
    SetEndColor(Color(1.0f,0.3f,0.0f,0.0f));

    SetBlendingMode(ADDITIVE_BLENDING_MODE);

    SetEmitPosition(position);
    SetEmitPositionVariance(Vector3f(0.0f,0.0f,0.0f));


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
