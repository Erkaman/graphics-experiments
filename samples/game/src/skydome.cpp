#include "skydome.hpp"
#include "biased_random.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector4f.hpp"
#include "ewa/math/matrix4f.hpp"

#include "ewa/value_noise_seed.hpp"
#include "ewa/camera.hpp"

#include "ewa/common.hpp"
#include "ewa/util.hpp"
#include "ewa/random.hpp"
#include "ewa/mult_array.hpp"


using std::vector;
using std::string;

constexpr GLushort NUM_BILLBOARD_TRIANGLES = 2;
constexpr int NUM_CLOUD_TEXTURES = 1; // 9

constexpr int CLOUD_GRID_X_SIZE = 20;
constexpr int CLOUD_GRID_Y_SIZE = 20;

// elevation
constexpr int CLOUD_GRID_X_COUNT = (180 / CLOUD_GRID_X_SIZE); // = 9
// rotation
constexpr int CLOUD_GRID_Y_COUNT = (360 / CLOUD_GRID_Y_SIZE); // = 9

struct CloudInfo {
public:

    float m_orientation;
    float m_elevation;
    float m_rotation;

    float m_orientationVelocity;
    float m_elevationVelocity;
    float m_rotationVelocity;


    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;
};

struct CloudGroup{
public:

    Texture* m_cloudTexture;
    vector<CloudInfo> m_clouds;
    GLsizei m_width;
    GLsizei m_height;

};

void GenerateBillboardVertices(VBO* m_vertexBuffer, VBO* m_indexBuffer, const float width, const float height);

Skydome::Skydome(const float radius, const int slices, const int stacks): GeometryObject(Vector3f(0), Vector3f(1)),m_rng(new Random(3)) {

    /*
      Create the skydome.
    */


    MakeSky(radius, slices, stacks);

    m_billboardShader = new ShaderProgram("shader/billboard");

    MakeSun();
    MakeClouds();

}

void Skydome::MakeSky(const float radius, const int slices, const int stacks) {
    m_domeShader = new ShaderProgram("shader/skydome");

    m_domeVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,3}
	);
    m_domeIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);
    m_domeNumTriangles = GenerateSphereVertices(radius, slices, stacks, m_domeVertexBuffer, m_domeIndexBuffer);

}

void Skydome::MakeClouds() {


    /*
      First we load all texture.
     */

    for(int i = 0; i < NUM_CLOUD_TEXTURES; ++i) {

	CloudGroup* cloudGroup = new CloudGroup();

	string textureFilename = string("img/cloud") + std::to_string(i) + string(".png");
	Texture* cloudTexture= new Texture2D(textureFilename);

	cloudGroup->m_width = cloudTexture->GetWidth();
	cloudGroup->m_height = cloudTexture->GetHeight();


//	LOG_I("width, height = %ld,%ld", cloudTexture->GetWidth(), cloudTexture->GetHeight());

	cloudTexture->Bind();
	cloudTexture->SetTextureClamping();
	cloudTexture->SetMinFilter(GL_LINEAR);
	cloudTexture->SetMagFilter(GL_NEAREST);
	cloudTexture->Unbind();

	cloudGroup->m_cloudTexture= cloudTexture;

	m_clouds.push_back(cloudGroup);

    }

    /*
      Next we place out the clouds.
     */

    MultArray<bool> cloudGrid(CLOUD_GRID_X_COUNT, CLOUD_GRID_Y_COUNT, false);

    const int numClouds = 90;//m_rng->RandomInt(30,40);

    vector<int> items;
    vector<float> weights;

    int middleX = (CLOUD_GRID_X_COUNT-1)/2;

    for(int x = 0; x <= (CLOUD_GRID_X_COUNT-1); ++x) {

	float weight = abs(middleX -x);
	weights.push_back(weight);
	items.push_back(x);
    }
    // make sure the clouds don't gather at the top.

    for(int y = 0; y < CLOUD_GRID_Y_COUNT; ++y) {

	constexpr int S = 0;
	for(int x = (middleX-S); x <= (middleX+S); ++x) {
	    cloudGrid(x,y) = true;
	}
    }


//    BiasedRandom<int> bias(m_rng, items, weights);

    for(int i = 0; i < numClouds; ++i) {

	CloudInfo cloudInfo;

	// randomize cloud group.
	CloudGroup* cloudGroup = m_clouds[/*8*/ m_rng->RandomInt(0, NUM_CLOUD_TEXTURES-1)];

	cloudInfo.m_vertexBuffer = VBO::CreateInterleaved(
	    vector<GLuint>{
		VBO_POSITION_ATTRIB_INDEX,
		    VBO_TEX_COORD_ATTRIB_INDEX },
	    vector<GLuint>{3,2}
	    );

	cloudInfo.m_indexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

	const float cloudWidth = 0.1 + cloudGroup->m_width / 10000.0f;
	const float cloudHeight = 0.1 + cloudGroup->m_height / 10000.0f;

	GenerateBillboardVertices(cloudInfo.m_vertexBuffer, cloudInfo.m_indexBuffer,  cloudWidth, cloudHeight);

	// now place out the cloud in the cloud grid.

	// first we find an empty grid.
	int gridX;
	int gridY;
	while(true) {

	    gridX = /*bias.GetRandomItem();*/ m_rng->RandomInt(0, CLOUD_GRID_X_COUNT-1);
	    gridY = m_rng->RandomInt(0, CLOUD_GRID_Y_COUNT-1);


	    if(cloudGrid(gridX, gridY) == false) {
	    	cloudGrid(gridX,gridY) = true;
		break; // found an empty grid position.
	    }
	}

	cloudInfo.m_elevation =
	    CLOUD_GRID_X_SIZE * gridX +
	    m_rng->RandomFloat(-CLOUD_GRID_X_SIZE/2, CLOUD_GRID_X_SIZE/2);
	cloudInfo.m_rotation =
	    CLOUD_GRID_Y_SIZE * gridY +
	    m_rng->RandomFloat(-CLOUD_GRID_Y_SIZE/2, CLOUD_GRID_Y_SIZE/2);
	cloudInfo.m_orientation = m_rng->RandomFloat(0.0f,360.0f);

	cloudInfo.m_elevationVelocity = m_rng->RandomFloat(-0.1,+0.1);
	cloudInfo.m_rotationVelocity = 0.2f  +  m_rng->RandomFloat(-0.1,+0.1);
	cloudInfo.m_orientationVelocity = m_rng->RandomFloat(-1.5,+1.5);


	cloudGroup->m_clouds.push_back(cloudInfo);

    }
}

void Skydome::MakeSun() {

    m_sunVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{
	    VBO_POSITION_ATTRIB_INDEX,
		VBO_TEX_COORD_ATTRIB_INDEX },
	vector<GLuint>{3,2}
	);

    m_sunIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    GenerateBillboardVertices(m_sunVertexBuffer, m_sunIndexBuffer, 0.06f, 0.06f);

    m_sunTexture = new Texture2D("img/sun.png");

    m_sunTexture->Bind();
    m_sunTexture->SetTextureClamping();
    m_sunTexture->SetMinFilter(GL_LINEAR);
    m_sunTexture->SetMagFilter(GL_NEAREST);
    m_sunTexture->Unbind();

}

Skydome::~Skydome() {
    MY_DELETE(m_domeShader);
    MY_DELETE(m_domeVertexBuffer);
    MY_DELETE(m_domeIndexBuffer);

    MY_DELETE(m_billboardShader);
    MY_DELETE(m_sunVertexBuffer);
    MY_DELETE(m_sunIndexBuffer);
    MY_DELETE(m_sunTexture);
    MY_DELETE(m_rng)
}

void Skydome::Draw(const Camera& camera) {

    SetDepthTest(false);


    DrawDome(camera);


    // next we'll draw all the billboards:

    m_billboardShader->Bind();
    GL_C(glEnable(GL_BLEND)); // all the billboards use alpha blending.
    GL_C(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    DrawSun(camera);


    DrawClouds(camera);

    // done drawing billboards.
    GL_C(glDisable(GL_BLEND));
    m_billboardShader->Unbind();


    SetDepthTest(true);
}

void Skydome::Update(const float delta) {

    for(CloudGroup* cloudGroup : m_clouds ) {

	for(CloudInfo& cloud : cloudGroup->m_clouds) {

	    // update a single cloud.

	    cloud.m_orientation += cloud.m_orientationVelocity * delta;
	    cloud.m_rotation += cloud.m_rotationVelocity * delta;
	    cloud.m_elevation += cloud.m_elevationVelocity * delta;


	}

	cloudGroup->m_cloudTexture->Unbind();
    }
}

void GenerateBillboardVertices(VBO* m_vertexBuffer, VBO* m_indexBuffer, const float width, const float height) {

    FloatVector vertices;
    UshortVector indices;

    Vector3f sunPoint(0, 0, 1);
    Vector3f xaxis(1, 0, 0);
    Vector3f yaxis(0, 1, 0);

    (sunPoint - width * xaxis + height * yaxis).Add(vertices);
    Vector2f(0.0f,1.0f).Add(vertices);

    (sunPoint + width * xaxis + height * yaxis).Add(vertices);
    Vector2f(1.0f,1.0f).Add(vertices);

    (sunPoint - width * xaxis - height * yaxis).Add(vertices);
    Vector2f(0.0f,0.0f).Add(vertices);

    (sunPoint + width * xaxis - height * yaxis).Add(vertices);
    Vector2f(1.0f,0.0f).Add(vertices);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(2);

    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();
}

void Skydome::DrawDome(const Camera& camera) {
    m_domeShader->Bind();

    Matrix4f modelView =  camera.GetViewMatrix();
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_domeShader->SetUniform("mvp", mvp);

    VBO::DrawIndices(*m_domeVertexBuffer, *m_domeIndexBuffer, GL_TRIANGLES, (m_domeNumTriangles)*3);

    m_domeShader->Unbind();


}

void Skydome::DrawSun(const Camera& camera) {

    m_sunTexture->Bind();

    DrawBillboard(camera, m_sunVertexBuffer, m_sunIndexBuffer, -10.0f,5.0f,0.0f);

    m_sunTexture->Unbind();
}


void Skydome::DrawBillboard(const Camera& camera, VBO* m_vertexBuffer, VBO* m_indexBuffer,
			    const float orientation, const float elevation, const float rotation) {


    const Matrix4f orientationMatrix =Matrix4f::CreateRotate(orientation, Vector3f(0,0,1) );
    const Matrix4f elevationMatrix =Matrix4f::CreateRotate(-elevation, Vector3f(1,0,0) );
    const Matrix4f rotationMatrix = Matrix4f::CreateRotate(rotation, Vector3f(0,1,0) );

    const Matrix4f model = rotationMatrix * elevationMatrix * orientationMatrix;


    Matrix4f modelView =  camera.GetModelViewMatrix(model);
    modelView.m03 = 0;
    modelView.m13 = 0;
    modelView.m23 = 0;

    const Matrix4f mvp = camera.GetProjectionMatrix() * modelView;

    m_billboardShader->SetUniform("mvp", mvp);

    m_billboardShader->SetUniform("tex", 0);
    Texture::SetActiveTextureUnit(0);

    VBO::DrawIndices(*m_vertexBuffer, *m_indexBuffer, GL_TRIANGLES, (NUM_BILLBOARD_TRIANGLES)*3);

}

void Skydome::DrawClouds(const Camera& camera) {


    for(const CloudGroup* cloudGroup : m_clouds ) {

	cloudGroup->m_cloudTexture->Bind();

	for(const CloudInfo& cloud : cloudGroup->m_clouds) {

	    DrawBillboard(camera, cloud.m_vertexBuffer, cloud.m_indexBuffer,
			  cloud.m_orientation, cloud.m_elevation, cloud.m_rotation);

	}

	cloudGroup->m_cloudTexture->Unbind();
    }
}
