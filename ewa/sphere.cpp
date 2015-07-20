#include "sphere.hpp"

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

using std::vector;

GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    );

Sphere::Sphere(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)), m_delta(0),     m_azimuthAngle(-27.7f), m_elevationAngle(2.74f),m_reileighCoefficient(1.0f),m_mieCoefficient(0.053f), m_mieDirectionalG(0.75f),m_turbidity(1.0f)
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

    m_numTriangles = GenerateVertices(radius, slices, stacks, m_vertexBuffer, m_indexBuffer);

    m_perlinSeed = new ValueNoiseSeed(2);

    UpdateSunDirection();
}



Sphere::~Sphere() {
    MY_DELETE(m_shader);
    MY_DELETE(m_vertexBuffer);
    MY_DELETE(m_indexBuffer);
    MY_DELETE(m_perlinSeed);

}

void Sphere::Draw(const Camera& camera) {

    m_shader->Bind();

    SetDepthTest(false);

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_shader->SetUniform("mvp", mvp);
//    m_shader->SetUniform("sampler", 0);
    m_shader->SetUniform("delta", m_delta);

//    LOG_I("delta: %f", m_delta);


    m_shader->SetUniform("sunDirection", m_sunDirection);

    m_shader->SetUniform("reileighCoefficient", m_reileighCoefficient);
    m_shader->SetUniform("mieCoefficient", m_mieCoefficient);
    m_shader->SetUniform("mieDirectionalG", m_mieDirectionalG);
    m_shader->SetUniform("turbidity", m_turbidity);

    m_shader->SetUniform("cameraPosition", camera.GetPosition());


    m_perlinSeed->Bind(*m_shader);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (m_numTriangles)*3);

    m_perlinSeed->Unbind();


    m_shader->Unbind();

    SetDepthTest(true);

}


GLushort GenerateVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    ) {

    FloatVector vertices;
    UshortVector indices;

    // loop through stacks.
    for (int i = 0; i <= stacks; ++i){

	float V   = (float)i / (float) stacks;
	float phi = V * PI;

	// loop through the slices.
	for (int j = 0; j <= slices; ++j){

	    float U = (float)j / (float) slices;
	    float theta = U * (PI * 2);

	    // use spherical coordinates to calculate the positions.
	    float x = cos (theta) * sin (phi);
	    float y = cos (phi);
	    float z = sin (theta) * sin (phi);

	    Vector3f pos(x,y,z);
	    pos *= radius;
	    pos.Add(vertices);

//	    Color(x,y,z).Add(vertices);
	    Vector3f(x,y,z).Add(vertices);

//	    LOG_I("Y: %f", y);

	}
    }

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){

	indices.push_back (i + slices);
	indices.push_back (i + slices + 1);
	indices.push_back (i);

	indices.push_back (i + 1);
	indices.push_back (i);
	indices.push_back (i + slices + 1);
    }


//    LOG_I("SPHERE");

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    return indices.size();
}

void Sphere::Update(const float delta) {
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

    if(kbs.IsPressed(GLFW_KEY_B)) {
	m_reileighCoefficient += 0.05 * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_N)) {
	m_reileighCoefficient -= 0.05 * SPEED;
    }


    if(kbs.IsPressed(GLFW_KEY_C)) {
	m_mieCoefficient += 0.001 * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_V)) {
	m_mieCoefficient -= 0.001 * SPEED;
    }

    if(kbs.IsPressed(GLFW_KEY_Z)) {
	m_mieDirectionalG += 0.01 * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_X)) {
	m_mieDirectionalG -= 0.01 * SPEED;
    }


    if(kbs.IsPressed(GLFW_KEY_1)) {
	m_turbidity += 0.01 * SPEED;
    }
    if(kbs.IsPressed(GLFW_KEY_2)) {
	m_turbidity -= 0.01 * SPEED;
    }

}

void Sphere::UpdateSunDirection() {

    const Matrix4f elevation =Matrix4f::CreateRotate(m_elevationAngle, Vector3f(0,0,1) ); //glm::rotate(elevationAngle, vec3(0, 0, 1));
    const Matrix4f rotation = Matrix4f::CreateRotate(m_azimuthAngle, Vector3f(0,1,0) );
//    const Matrix4f rotation = glm::rotate(azimuthAngle, vec3(0, 1, 0));

    m_sunDirection = Vector3f(rotation * elevation * Vector4f(1, 0, 0, 1));

}
