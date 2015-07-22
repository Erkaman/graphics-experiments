#include "skydome.hpp"

#include "gl/vbo.hpp"
#include "gl/texture2d.hpp"
#include "gl/shader_program.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

#include "value_noise_seed.hpp"
#include "keyboard_state.hpp"

#include "common.hpp"
#include "camera.hpp"

#include "gl/gl_common.hpp"

#include "ewa/util.hpp"


using std::vector;

Skydome::Skydome(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)), m_delta(0),     m_azimuthAngle(-27.7f), m_elevationAngle(70.74f),m_reileighCoefficient(1.0f),m_mieCoefficient(0.053f), m_mieDirectionalG(0.75f),m_turbidity(1.0f)
 {

    /*
      load the shader
    */

    m_shader = new ShaderProgram("shader/sphere");

    m_vertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,3}
	);

    m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_numTriangles = GenerateSphereVertices(radius, slices, stacks, m_vertexBuffer, m_indexBuffer);

    UpdateSunDirection();
}



Skydome::~Skydome() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
}

void Skydome::Draw(const Camera& camera) {

    m_shader->Bind();

    SetDepthTest(false);

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
    m_shader->SetUniform("delta", m_delta);

    m_shader->SetUniform("sunDirection", m_sunDirection);

    m_shader->SetUniform("reileighCoefficient", m_reileighCoefficient);
    m_shader->SetUniform("mieCoefficient", m_mieCoefficient);
    m_shader->SetUniform("mieDirectionalG", m_mieDirectionalG);
    m_shader->SetUniform("turbidity", m_turbidity);

    m_shader->SetUniform("cameraPosition", camera.GetPosition());


    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_shader->Unbind();

    SetDepthTest(true);

}



void Skydome::Update(const float delta) {
    m_delta += delta;

/*    if(GetKey( GLFW_KEY_U ) == GLFW_PRESS) {
	m_elevationAngle += delta * 0.5f;
	}*/

    const KeyboardState& kbs = KeyboardState::GetInstance();

    const float SPEED = 5.5f;

    if(kbs.IsPressed(GLFW_KEY_U)) {
	m_elevationAngle += delta * SPEED;
	UpdateSunDirection();
    }
    if(kbs.IsPressed(GLFW_KEY_I)) {
	m_elevationAngle -= delta * SPEED;
	UpdateSunDirection();
    }

    if(kbs.IsPressed(GLFW_KEY_T)) {
	m_azimuthAngle += delta * SPEED;
	UpdateSunDirection();
    }
    if(kbs.IsPressed(GLFW_KEY_Y)) {
	m_azimuthAngle -= delta * SPEED;
	UpdateSunDirection();
    }

    // makes it less bright
    if(kbs.IsPressed(GLFW_KEY_B)) {
	m_reileighCoefficient += 0.05f * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_N)) {
	m_reileighCoefficient -= 0.05f * SPEED;
    }


    if(kbs.IsPressed(GLFW_KEY_C)) {
	m_mieCoefficient += 0.001f * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_V)) {
	m_mieCoefficient -= 0.001f * SPEED;
    }

    if(kbs.IsPressed(GLFW_KEY_Z)) {
	m_mieDirectionalG += 0.01f * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_X)) {
	m_mieDirectionalG -= 0.01f * SPEED;
    }


    if(kbs.IsPressed(GLFW_KEY_1)) {
	m_turbidity += 0.01f * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_2)) {
	m_turbidity -= 0.01f * SPEED;
    }

}

void Skydome::UpdateSunDirection() {

    const Matrix4f elevation =Matrix4f::CreateRotate(m_elevationAngle, Vector3f(0,0,1) ); //glm::rotate(elevationAngle, vec3(0, 0, 1));
    const Matrix4f rotation = Matrix4f::CreateRotate(m_azimuthAngle, Vector3f(0,1,0) );
//    const Matrix4f rotation = glm::rotate(azimuthAngle, vec3(0, 1, 0));

    m_sunDirection = Vector3f(rotation * elevation * Vector4f(1, 0, 0, 1));

}
