#include "fire_effect.hpp"

#include "ewa/gl/texture2d.hpp"

#include "ewa/log.hpp"


FireEffect::FireEffect(const Vector3f& position) {

    constexpr float V = 0.286f;

    constexpr float range = 0.05f;


    SetMinVelocity(Vector3f(-range,V,-range) );
    SetMaxVelocity(Vector3f(+range,V + 0.2,+range));
    /*
    SetMinVelocity(Vector3f(0,0,0) );
    SetMaxVelocity(Vector3f(0,0,0));
    */

    SetBaseParticleLifetime(0.9f);
    SetParticleLifetimeVariance(0.9f);
    /*
    SetBaseParticleLifetime(1000.0f);
    SetParticleLifetimeVariance(0.0f);
    */



    SetBaseStartSize(0.20f);
    SetBaseEndSize(0.09f);
    SetStartSizeVariance(0.06f);
    SetEndSizeVariance(0.03f);

/*
    SetBaseStartSize(0.80f);
    SetBaseEndSize(0.80f);
    SetStartSizeVariance(0.00f);
    SetEndSizeVariance(0.00f);
*/




    SetStartColor(Color(1.0f,0.3f,0.0f,0.6f));
    SetEndColor(Color(1.0f,0.3f,0.0f,0.0f));

    SetBlendingMode(ADDITIVE_BLENDING_MODE);

    SetEmitPosition(position);
    SetEmitPositionVariance(Vector3f(0.0f,0.0f,0.0f));



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
