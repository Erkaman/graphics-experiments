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

#include "particle_system.h"

#include "ewa/math/vector3f.hpp"
#include "ewa/common.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/query.hpp"

#include "ewa/random_texture.hpp"

#define MAX_PARTICLES 1000
#define PARTICLE_LIFETIME 10.0f

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f

using std::vector;

struct Particle
{
    float Type;
    Vector3f Pos;
    Vector3f Vel;
    float LifetimeMillis;
};

void beforeLinkingHook(GLuint shaderProgram) {
    const GLchar* Varyings[4];
    Varyings[0] = "Type1";
    Varyings[1] = "Position1";
    Varyings[2] = "Velocity1";
    Varyings[3] = "Age1";

    GL_C(glTransformFeedbackVaryings(shaderProgram, 4, Varyings, GL_INTERLEAVED_ATTRIBS));
}

ParticleSystem::~ParticleSystem()
{
    MY_DELETE(m_texture);
    MY_DELETE(m_randomTexture);

    if (m_transformFeedback[0] != 0) {
        glDeleteTransformFeedbacks(2, m_transformFeedback);
    }

    MY_DELETE(m_particleBuffer[0]);
    MY_DELETE(m_particleBuffer[1]);
}


ParticleSystem::ParticleSystem(const Vector3f& Pos)
{
    count = 0;

    m_currVB = 0;
    m_currTFB = 1;
    m_isFirst = true;
    m_time = 0;
    m_texture = NULL;


    Particle Particles[MAX_PARTICLES];

    Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
    Particles[0].Pos = Pos;
    Particles[0].Vel = Vector3f(0.0f, 0.0001f, 0.0f);
    Particles[0].LifetimeMillis = 0.0f;

    GL_C(glGenTransformFeedbacks(2, m_transformFeedback));

    for (unsigned int i = 0; i < 2 ; i++) {


	m_particleBuffer[i] = VBO::CreateInterleaved(
	    vector<GLuint>{1,3,3,1}, // type, pos, vel, lifetime
	    GL_DYNAMIC_DRAW
	);

        GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]));


	m_particleBuffer[i]->Bind();

	m_particleBuffer[i]->SetBufferData(sizeof(Particles), Particles);
	m_particleBuffer[i]->Unbind();

        GL_C(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]->GetBuffer()));
    }

    m_updateTechnique = new ShaderProgram("shader/update", beforeLinkingHook);

    m_updateTechnique->Bind();

    m_updateTechnique->SetUniform("gLauncherLifetime", 0.1f);
    m_updateTechnique->SetUniform("gShellLifetime", /*10.0f*/ 1000000000.0f);
    m_updateTechnique->SetUniform("gSecondaryShellLifetime", /*2.5f*/ 10000000.0f);

    m_updateTechnique->Unbind();

    m_randomTexture = new RandomTexture(1000, 2);


    m_billboardTechnique = new ShaderProgram("shader/part_billboard");

    m_billboardTechnique->Bind();


    m_texture = new Texture2D( "img/fireworks_red.png");

    m_texture->Bind();
    m_texture->SetTextureRepeat();
    m_texture->GenerateMipmap();
    m_texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_texture->SetMagFilter(GL_LINEAR);
    m_texture->Unbind();


    m_updateTechnique->Query();


    LOG_I("BEGIN");
    Print(m_currVB);
    Print(m_currTFB);
}

void ParticleSystem::Print(int i) {
    Particle arr[20];
//    m_particleBuffer[i]->Unbind();
//    m_particleBuffer[i]->Bind();

    // binding in the wrong way?


    m_particleBuffer[i]->GetBufferSubData(0, sizeof(arr), &arr);

    for(int i =0; i < 20; ++i) {

	Particle p = arr[0];

	LOG_I("%d age: %f, vel: %s, type: %f", i, p.LifetimeMillis, tos(p.Vel).c_str(), p.Type );
    }


}

void ParticleSystem::Update(float delta){
    m_time += delta;

    UpdateParticles(delta);
}

void ParticleSystem::Render(const Matrix4f& VP, const Vector3f& CameraPos){
    RenderParticles(VP, CameraPos);

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) & 0x1;
}

void ParticleSystem::UpdateParticles(float delta){

    m_updateTechnique->Bind();
    m_updateTechnique->SetUniform("gTime", m_time);
    m_updateTechnique->SetUniform("gDeltaTimeSecs", delta);


    m_updateTechnique->SetUniform("gRandomTexture", 0);
    Texture::SetActiveTextureUnit(0);
    m_randomTexture->Bind();


    GL_C(glEnable(GL_RASTERIZER_DISCARD));

    m_particleBuffer[m_currVB]->Bind();

    GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]));

    m_particleBuffer[m_currVB]->EnableVertexAttribInterleavedWithBind();

    // TODO: enabledinterleaved here!


//    Query query(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);


//    query.Bind();

    GL_C(glBeginTransformFeedback(GL_POINTS));
    // within begin/end, we'll render frmo currVB, and the rendered vertices will be captured in currTFB



    if (m_isFirst) {
	// VBO:: call draw arrays in VBO here
	m_particleBuffer[m_currVB]->DrawVertices(GL_POINTS, 1);
//        glDrawArrays(GL_POINTS, 0, 1);

        m_isFirst = false;
    }
    else {

	// m_transformFeedback[m_currVB] is the transform feedback buffer bound to the current vertex buffer. from that vb we fetch the number of vertices to draw.
        GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]));
    }

    GL_C(glEndTransformFeedback());

//    query.Unbind();

//    glFlush();

//    LOG_I("feedback result: %d", query.GetResult());



    m_particleBuffer[m_currVB]->DisableVertexAttribInterleavedWithBind();


    m_updateTechnique->Unbind();

    GL_C(glDisable(GL_RASTERIZER_DISCARD));
/*
    Print(m_currVB);
    Print(m_currTFB);
*/
}


void ParticleSystem::RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos)
{
/*    Print(m_currVB);
    Print(m_currTFB);
*/

    GL_C(glDisable(GL_RASTERIZER_DISCARD));

    m_billboardTechnique->Bind();
    m_billboardTechnique->SetUniform("gCameraPos", CameraPos);
    m_billboardTechnique->SetUniform("gVP", VP);
    m_billboardTechnique->SetUniform("gBillboardSize", 0.05f);

    m_billboardTechnique->SetUniform("gColorMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_texture->Bind();
//    m_pTexture->Bind(COLOR_TEXTURE_UNIT);


    m_particleBuffer[m_currTFB]->Bind();

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

    // instead of using normal drawelements we use glDrawTransformFeedback here instead!
    GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]));

    glDisableVertexAttribArray(0);

    m_billboardTechnique->Unbind();

    ++count;

    /*  if(count > 8) {
	LOG_I("OVER");
    Print(m_currVB);
    Print(m_currTFB);
    LOG_I("m_time. %f", m_time);
      exit(1);

      }*/
}
