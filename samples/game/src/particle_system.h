#ifndef PARTICLE_SYSTEM_H
#define	PARTICLE_SYSTEM_H

#include "ewa/gl/gl_common.hpp"

class Vector3f;
class Matrix4f;
class Matrix4f;
class Texture;
class ShaderProgram;
class VBO;
class RandomTexture;

class ParticleSystem
{
public:
    ParticleSystem(const Vector3f& Pos);

    ~ParticleSystem();

    void Render(const Matrix4f& VP, const Vector3f& CameraPos);
    void Update(float delta);

private:

    void UpdateParticles(float delta);

    void RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos);


    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    VBO* m_particleBuffer[2];

    GLuint m_transformFeedback[2];
    ShaderProgram* m_updateTechnique;
    ShaderProgram* m_billboardTechnique;
    Texture* m_texture;
    float m_time;

    RandomTexture* m_randomTexture;

    int count = 0;
};


#endif	/* PARTICLE_SYSTEM_H */
