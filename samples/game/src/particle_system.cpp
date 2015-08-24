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

    if (m_transformFeedback[0] != 0) {
        glDeleteTransformFeedbacks(2, m_transformFeedback);
    }



    MY_DELETE(m_particleBuffer[0]);
    MY_DELETE(m_particleBuffer[1]);
}


ParticleSystem::ParticleSystem(const Vector3f& Pos)
{
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

//    glGenBuffers(2, m_particleBuffer);

    for (unsigned int i = 0; i < 2 ; i++) {


	LOG_I("BEG");

	m_particleBuffer[i] = VBO::CreateInterleaved(
	    vector<GLuint>{1,3,3,1}, // type, pos, vel, lifetime
	    GL_DYNAMIC_DRAW
	);


	LOG_I("END");

        GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]));


	m_particleBuffer[i]->Bind();

//	LOG_I("size: %ld",sizeof(Particles) );

	m_particleBuffer[i]->SetBufferData(sizeof(Particles), Particles);
//	m_particleBuffer[i]->Unbind();







	// set vbo
//        glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);



        GL_C(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]->GetBuffer()));


    }

    m_updateTechnique = new ShaderProgram("shader/update", beforeLinkingHook);

    m_updateTechnique->Bind();

//    m_updateTechnique.SetRandomTextureUnit(RANDOM_TEXTURE_UNIT_INDEX);

    m_updateTechnique->SetUniform("gLauncherLifetime", 100.0f);
    m_updateTechnique->SetUniform("gShellLifetime", 10000.0f);
    m_updateTechnique->SetUniform("gSecondaryShellLifetime", 2500.0f);

    m_updateTechnique->Unbind();


/*    if (!m_randomTexture.InitRandomTexture(1000)) {
        return false;
	}*/

//    m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);

    m_billboardTechnique = new ShaderProgram("shader/part_billboard");


    m_billboardTechnique->Bind();

    m_billboardTechnique->SetUniform("gBillboardSize", 0.01f);

    m_texture = new Texture2D( "img/fireworks_red.png");

    m_texture->Bind();
    m_texture->SetTextureRepeat();
    m_texture->GenerateMipmap();
    m_texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    m_texture->SetMagFilter(GL_LINEAR);
    m_texture->Unbind();


    m_updateTechnique->Query();



}

void ParticleSystem::Print(int i) {

    // m_currVB
/*
    Particle arr[5];
           m_particleBuffer[i]->Unbind();
           m_particleBuffer[i]->Bind();

	// binding in the wrong way?


	m_particleBuffer[i]->GetBufferSubData(0, sizeof(arr), &arr);
	LOG_I("vel0: %s", tos(arr[0].Vel).c_str() );
	LOG_I("vel1: %s", tos(arr[1].Vel).c_str() );
	LOG_I("vel2: %s", tos(arr[2].Vel).c_str() );


	LOG_I("pos0: %s", tos(arr[0].Pos).c_str() );
	LOG_I("pos1: %s", tos(arr[1].Pos).c_str() );
	LOG_I("pos2: %s", tos(arr[2].Pos).c_str() );
*/

}

void ParticleSystem::Update(int DeltaTimeMillis){
//    LOG_I("UPDATE");
    m_time += DeltaTimeMillis;







    UpdateParticles(DeltaTimeMillis);
}

void ParticleSystem::Render(const Matrix4f& VP, const Vector3f& CameraPos){
//    LOG_I("RENDER");

    RenderParticles(VP, CameraPos);

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) & 0x1;
}


void ParticleSystem::UpdateParticles(int DeltaTimeMillis)
{
//    m_particleBuffer[m_currVB]->Bind();


//    m_particleBuffer[m_currVB]->Unbind();

    Print(m_currVB);
    Print(m_currTFB);





    m_updateTechnique->Bind();
    m_updateTechnique->SetUniform("gTime", m_time);
    m_updateTechnique->SetUniform("gDeltaTimeMillis", DeltaTimeMillis);

//    m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);

    GL_C(glEnable(GL_RASTERIZER_DISCARD));

    //  glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
    // TODO: BIND VBO Her.
    m_particleBuffer[m_currVB]->Bind();

    GL_C(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]));

    /*
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);         // position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);        // velocity
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);          // lifetime
    */
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

    //   query.Unbind();

//    glFlush();

//    LOG_I("feedback result: %d", query.GetResult());



    /*
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    */
    // TODO: disable interleaved here.
    m_particleBuffer[m_currVB]->DisableVertexAttribInterleavedWithBind();


    m_updateTechnique->Unbind();

    GL_C(glDisable(GL_RASTERIZER_DISCARD));

    Print(m_currVB);
    Print(m_currTFB);






}


void ParticleSystem::RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos)
{
    Print(m_currVB);
    Print(m_currTFB);


    GL_C(glDisable(GL_RASTERIZER_DISCARD));

    m_billboardTechnique->Bind();
    m_billboardTechnique->SetUniform("gCameraPos", CameraPos);
    m_billboardTechnique->SetUniform("gVP", VP);

    m_billboardTechnique->SetUniform("gColorMap", 0);
    Texture::SetActiveTextureUnit(0);
    m_texture->Bind();
//    m_pTexture->Bind(COLOR_TEXTURE_UNIT);


//    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);
    m_particleBuffer[m_currTFB]->Bind();

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

    // instead of using normal drawelements we use glDrawTransformFeedback here instead!
    GL_C(glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]));

    glDisableVertexAttribArray(0);

    m_billboardTechnique->Unbind();

//    exit(1);
}

/*
this is probably useful:
TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
*/

/*
In either separate or interleaved modes, all transform feedback binding points
that will be written to must have buffer objects bound when BeginTransformFeedback is called

 */


/*

  two possibilities:
  1. either something is wrong in the billboard shader, so the particles don't show.
  2. or, the particle shader is never run in the first place, because the transform feedback buffer is empty or something.

 */
