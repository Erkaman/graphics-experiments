/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARTICLE_SYSTEM_H
#define	PARTICLE_SYSTEM_H

#include "ewa/gl/gl_common.hpp"

//#include <GL/glew.h>



class Vector3f;
class Matrix4f;
class Matrix4f;
class Texture;
class ShaderProgram;
class VBO;

class ParticleSystem
{
public:
    ParticleSystem(const Vector3f& Pos);

    ~ParticleSystem();

    void Render(const Matrix4f& VP, const Vector3f& CameraPos);
    void Update(float delta);
    void Print(int i);

private:

    void UpdateParticles(float delta);

    void RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos);


    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
//    GLuint m_particleBuffer[2];
    VBO* m_particleBuffer[2];

    GLuint m_transformFeedback[2];
    ShaderProgram* m_updateTechnique;
    ShaderProgram* m_billboardTechnique;
    //RandomTexture m_randomTexture;
    Texture* m_texture;
    float m_time;

    int count = 0;
};


#endif	/* PARTICLE_SYSTEM_H */
