#include "lighting_pass.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/vbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/config.hpp"

#include "gbuffer.hpp"
#include "ewa/gl/depth_fbo.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/cube_map_texture.hpp"
#include "ewa/gl/color_depth_fbo.hpp"
#include "ewa/gl/color_fbo.hpp"

#include "ewa/common.hpp"


#include "ewa/math/vector4f.hpp"


#include "ewa/camera.hpp"

#include <float.h>


constexpr int SLICES = 10;
constexpr int STACKS = 10;

constexpr int GRID_COUNT = 8; // 1,2,4,8,16,32

using std::vector;
using std::string;

GLushort MyGenerateSphereVertices(
    const float radius, const int slices, const int stacks,

    VBO* m_vertexBuffer,
    VBO* m_indexBuffer
    ) {

    FloatVector vertices;
    UshortVector indices;

    vector<Vector3f> vs;


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
//	    pos *= radius;
	    pos.Add(vertices);

//	    Vector3f(x,y,z).Add(vertices);
//	    vs.push_back(Vector3f(x,y,z) );


//	    LOG_I("lol: %s",  string(Vector3f(x,y,z)).c_str()  );


	}
    }
    //  exit(1);

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i){

	indices.push_back (GLushort(i) );
	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i + slices));

	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i));
	indices.push_back (GLushort(i + 1));

//	LOG_I("short: %d", GLushort(i + slices + 1));

	/*
	indices.push_back (GLushort(i + slices));
	indices.push_back (GLushort(i + slices + 1));
	indices.push_back (GLushort(i));

	indices.push_back (GLushort(i + 1));
	indices.push_back (GLushort(i));
	indices.push_back (GLushort(i + slices + 1));
	*/
    }


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(vertices);
    m_vertexBuffer->Unbind();

    m_indexBuffer->Bind();
    m_indexBuffer->SetBufferData(indices);
    m_indexBuffer->Unbind();

    return (GLushort)indices.size();
}



LightingPass::LightingPass(int framebufferWidth, int framebufferHeight) {
    m_screenSize = Vector2f(framebufferWidth, framebufferHeight);

    m_directionalShader = ShaderProgram::Load("shader/directional");


    m_pointShader = ShaderProgram::Load("shader/point_light");

    m_sphereVertexBuffer = VBO::CreateInterleaved(
	vector<GLuint>{3} // pos
	);
    m_sphereIndexBuffer = VBO::CreateIndex(GL_UNSIGNED_SHORT);

    m_sphereNumTriangles = MyGenerateSphereVertices(1, SLICES, STACKS, m_sphereVertexBuffer, m_sphereIndexBuffer);


    m_lightGridTexture = new Texture2D(nullptr, GRID_COUNT, GRID_COUNT,
				       GL_RGB16F,
				       GL_RGB,
				       GL_FLOAT);


    m_lightGridTexture->Bind();
    m_lightGridTexture->SetTextureClamping();
    m_lightGridTexture->SetMinFilter(GL_NEAREST);
    m_lightGridTexture->SetMagFilter(GL_NEAREST);
    m_lightGridTexture->Unbind();

    m_lightGridTextureBuffer = new Vector3f[GRID_COUNT*GRID_COUNT];

    m_lightIndexTexture = NULL;
    m_lightIndexTextureBuffer = NULL;

}

void LightingPass::Render(
    Gbuffer* gbuffer, const ICamera* camera, const Vector4f& lightPosition,
    const Matrix4f& lightVp, const DepthFBO& shadowMap, const std::vector<Vector3f>& torches,
    CubeMapTexture* cubeMapTexture, ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap) {

    SetupShader(m_directionalShader, gbuffer, camera, cubeMapTexture, refractionMap, reflectionMap);


    m_directionalShader->SetUniform("shadowMap", SHADOW_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(SHADOW_TEXTURE_UNIT);
    shadowMap.GetRenderTargetTexture().Bind();


    m_directionalShader->SetUniform("lightGrid", LIGHT_GRID_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(LIGHT_GRID_TEXTURE_UNIT);
    m_lightGridTexture->Bind();




    m_directionalShader->SetLightUniforms(camera, lightPosition, lightVp);


    m_directionalShader->SetUniform("gridCount", (float)GRID_COUNT );


    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    shadowMap.GetRenderTargetTexture().Unbind();
    m_lightGridTexture->Unbind();

    UnsetupShader(m_directionalShader, gbuffer, cubeMapTexture, refractionMap, reflectionMap);

    shadowMap.GetRenderTargetTexture().Unbind();

    SetupShader(m_pointShader, gbuffer, camera, cubeMapTexture, refractionMap, reflectionMap);


    GL_C(glFrontFace(GL_CW));
    GL_C(glDisable(GL_DEPTH_TEST));
/*
    GL_C(glEnable(GL_BLEND));
    GL_C(glBlendFunc(GL_ONE, GL_ONE));
*/

    std::vector<PointLight> lights = GetTestLights();

//    std::vector<PointLight> lights = GetTorches(camera, torches);


    DrawLights(camera, lights);

    //  GL_C(glDisable(GL_BLEND));
    GL_C(glFrontFace(GL_CCW));
    GL_C(glEnable(GL_DEPTH_TEST));

    UnsetupShader(m_pointShader, gbuffer, cubeMapTexture, refractionMap, reflectionMap);
}

void LightingPass::DrawLights(const ICamera* camera, const std::vector<PointLight>& lights) {

    Matrix4f viewProjectionMatrix = camera->GetVp();

    MultArray<vector<int> > lightGrid(GRID_COUNT, GRID_COUNT);

    // compute AABB for every light source.
    for(int iPointLight = 0; iPointLight < lights.size(); ++iPointLight) {

	const PointLight& pointLight = lights[iPointLight];

	// first compute the AABB of the sphere.

	// the corners of the aabb.
	Vector3f corners[8];

	const Vector3f center = pointLight.m_position;
	const float radius = pointLight.m_radius;

	// positive y
	corners[0] = center + Vector3f(+radius, +radius, +radius);
	corners[1] = center + Vector3f(-radius, +radius, +radius);
	corners[2] = center + Vector3f(+radius, +radius, -radius);
	corners[3] = center + Vector3f(-radius, +radius, -radius);

	// negative y
	corners[4] = center + Vector3f(+radius, -radius, +radius);
	corners[5] = center + Vector3f(-radius, -radius, +radius);
	corners[6] = center + Vector3f(+radius, -radius, -radius);
	corners[7] = center + Vector3f(-radius, -radius, -radius);

	// project AABB corners to screen-space, then find the min and max AABB corners of the resulting 2D AABB.

	Vector2f aabbMin = Vector2f(FLT_MAX, FLT_MAX);
	Vector2f aabbMax = Vector2f(-FLT_MAX, -FLT_MAX);

	for(int i = 0; i < 8; ++i) {

	    Vector4f nonNormalized = viewProjectionMatrix * Vector4f(corners[i], 1.0);
	    Vector3f normalized = Vector3f(
		nonNormalized.x / nonNormalized.w,
		nonNormalized.y / nonNormalized.w,
		nonNormalized.z / nonNormalized.w
		);

//	    LOG_I("normalized.x: %f", normalized.x);

	    if(normalized.x < aabbMin.x) {
		aabbMin.x = normalized.x;
	    }

	    if(normalized.y < aabbMin.y) {
		aabbMin.y = normalized.y;
	    }

	    if(normalized.x > aabbMax.x) {
		aabbMax.x = normalized.x;

		//	LOG_I("set: %f", normalized.x);

	    }

	    if(normalized.y > aabbMax.y) {
		aabbMax.y = normalized.y;
	    }


//	    LOG_I("normx: %f", normalized.x);


	}

/*
	LOG_I("bla minX: %f", aabbMin.x);
	LOG_I("bla maxX: %f", aabbMax.x);

	LOG_I("bla minY: %f", aabbMin.y);
	LOG_I("bla maxY: %f", aabbMax.y);
*/


	// max.x cant get negative for some reason. always >= 0
//	LOG_I("bef: %f", aabbMax.x);

	// remap from [-1,+1] to [0,1], in order to simplify calculations.
	aabbMin = (aabbMin + 1.0) * 0.5f;
	aabbMax = (aabbMax + 1.0) * 0.5f;

	aabbMin.x = Clamp(aabbMin.x, 0.0f, 1.0f);
	aabbMin.y = Clamp(aabbMin.y, 0.0f, 1.0f);


	aabbMax.x = Clamp(aabbMax.x, 0.0f, 1.0f);
	aabbMax.y = Clamp(aabbMax.y, 0.0f, 1.0f);

/*
	LOG_I("bla minX: %f", aabbMin.x);
	LOG_I("bla maxX: %f", aabbMax.x);

	LOG_I("bla minY: %f", aabbMin.y);
	LOG_I("bla maxY: %f", aabbMax.y);
*/

//	exit(1);

	int gridMinX = aabbMin.x / (1.0 / GRID_COUNT);
	int gridMaxX = aabbMax.x / (1.0 / GRID_COUNT);

	int gridMinY = aabbMin.y / (1.0 / GRID_COUNT);
	int gridMaxY = aabbMax.y / (1.0 / GRID_COUNT);

	gridMaxX = gridMaxX >= GRID_COUNT ? GRID_COUNT-1 : gridMaxX;
	gridMaxY = gridMaxY >= GRID_COUNT ? GRID_COUNT-1 : gridMaxY;

	int count = (gridMaxX - gridMinX + 1) * (gridMaxY - gridMinY + 1);

	// now add the light source to the affected grid cells.
	for(int x = gridMinX; x <= gridMaxX; ++x) {

	    for(int y = gridMinY; y <= gridMaxY; ++y) {

		lightGrid(x,y).push_back(iPointLight);

	    }
	}
    }


    int lightIndex = 0;

    // create light grid texture.

    for(int y = 0; y < GRID_COUNT; ++y) {


	for(int x = 0; x < GRID_COUNT; ++x) {

	    int gridIndex = x + GRID_COUNT * y;

	    int offset = lightIndex;
	    int count = 0;

	    vector<int> lights = lightGrid(x,y);

	    for(int light : lights) {
		m_lightIndexTextureBuffer[lightIndex++] = light;
		++count;
	    }

	    // offset
	    // count
//	    m_lightGridTextureBuffer[gridIndex] = lightGrid(x,y).size() > 0 ? Vector3f(1,0,0) : Vector3f(0,0,0);
	    m_lightGridTextureBuffer[gridIndex] = Vector3f(offset,count,0);

	    //   LOG_I("%d", i);
	}

    }

    m_lightGridTexture->Bind();
    m_lightGridTexture->UpdateTexture( &m_lightGridTextureBuffer[0] );
    m_lightGridTexture->Unbind();

//    m_lightGridTextureBuffer

    /*
      tile light index list
      Put all the Vector<int>s in a flattened array.
     */

    /*
      global light list. (list of uniforms)
     */


    for(const PointLight& pointLight : lights) {
	DrawPointLight(
	    camera, pointLight.m_position,  pointLight.m_color, pointLight.m_radius);
    }
}

std::vector<PointLight> LightingPass::GetTorches(const ICamera* camera, const std::vector<Vector3f>& torches) {

    std::vector<PointLight> lights;

    for(Vector3f torch : torches ) {
	lights.push_back( PointLight(torch, Vector3f(0.4,0.4,0), 20.0f  ));
    }

    return lights;
}

std::vector<PointLight> LightingPass::GetTestLights() {
    int MIN_X = -2;
    int MAX_X = +2;

    int MIN_Z = -2;
    int MAX_Z = +2;

    std::vector<PointLight> lights;

/*
    for(int x = MIN_X; x <= MAX_X; ++x) {

	for(int z = MIN_Z; z <= MAX_Z; ++z) {


	    int mod = ((x-MIN_X)+(z-MIN_Z)) % 3;


	    float g;

	    if(mod == 0)
		g = 0;
	    else if(mod == 1)
		g = 0.5;
	    else
		g = 1;

	lights.push_back( PointLight(

			      Vector3f(40 * x,4, 40 * z),

			      Vector3f((float)(x-MIN_X) / (MAX_X-MIN_X),
			       g,
			       (float)(z-MIN_Z) / (MAX_Z-MIN_Z)
			      ),

			      30.0f));


	}

    }
*/




	lights.push_back( PointLight(

			      Vector3f(0,4, 0),

			      Vector3f(1,0,0
			      ),

			      30.0f));


    return lights;
}

void LightingPass::SetupShader(
    ShaderProgram* shader, Gbuffer* gbuffer, const ICamera* camera,
    CubeMapTexture* cubeMapTexture, ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap) {

    shader->Bind();

    shader->SetUniform("colorTexture", COLOR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(COLOR_TEXTURE_UNIT);
    gbuffer->GetColorTexture()->Bind();

    shader->SetUniform("depthTexture", DEPTH_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(DEPTH_TEXTURE_UNIT);
    gbuffer->GetDepthTexture()->Bind();

    shader->SetUniform("normalTexture", NORMAL_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(NORMAL_TEXTURE_UNIT);
    gbuffer->GetNormalTexture()->Bind();

    shader->SetUniform("specularTexture", SPECULAR_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(SPECULAR_TEXTURE_UNIT);
    gbuffer->GetSpecularTexture()->Bind();

    shader->SetUniform("envMap",ENV_TEXTURE_UNIT );
    Texture::SetActiveTextureUnit(ENV_TEXTURE_UNIT);
    cubeMapTexture->Bind();

    shader->SetUniform("refractionMap",REFRACTION_TEXTURE_UNIT );
    Texture::SetActiveTextureUnit(REFRACTION_TEXTURE_UNIT);
    refractionMap.GetColorTexture()->Bind();

    shader->SetUniform("reflectionMap", REFLECTION_TEXTURE_UNIT);
    Texture::SetActiveTextureUnit(REFLECTION_TEXTURE_UNIT);
    reflectionMap.GetRenderTargetTexture().Bind();


    shader->SetUniform("screenSize", m_screenSize);


    Config& config = Config::GetInstance();

    shader->SetUniform("ambientLight", config.GetAmbientLight() );
    shader->SetUniform("sceneLight", config.GetSceneLight() );

    Matrix4f invProj = camera->GetProjectionMatrix();
    invProj = invProj.Inverse();

    //  LOG_I("onv: %s", std::string(invProj).c_str() );

    shader->SetUniform("invProj", invProj);
    shader->SetUniform("proj", camera->GetProjectionMatrix());


    Matrix4f viewMatrix = camera->GetViewMatrix();
    Matrix4f invViewMatrix = viewMatrix.Inverse();
    shader->SetUniform("invViewMatrix", invViewMatrix  );

    shader->SetUniform("inverseViewNormalMatrix",
		       camera->GetViewMatrix().Transpose()  );

    shader->SetUniform("eyePos", camera->GetPosition() );
}

void LightingPass::UnsetupShader(
    ShaderProgram* shader, Gbuffer* gbuffer,
    CubeMapTexture* cubeMapTexture, ColorDepthFbo& refractionMap, const ColorFBO& reflectionMap) {
    shader->Unbind();

    gbuffer->GetColorTexture()->Unbind();
    gbuffer->GetDepthTexture()->Unbind();
    gbuffer->GetSpecularTexture()->Unbind();
    cubeMapTexture->Unbind();

    refractionMap.GetColorTexture()->Unbind();
    reflectionMap.GetRenderTargetTexture().Unbind();




}

void LightingPass::DrawPointLight(const ICamera* camera, const Vector3f& position, const Vector3f& color, float radius) {

    const Matrix4f modelViewMatrix =
	camera->GetViewMatrix() *

	Matrix4f::CreateTranslation( position ) *
	Matrix4f::CreateScale(radius,radius,radius);

    const Matrix4f mvp = camera->GetProjectionMatrix() * modelViewMatrix;


    m_pointShader->SetUniform("mvp",  mvp);
    m_pointShader->SetUniform("modelViewMatrix",  modelViewMatrix);
    m_pointShader->SetUniform("radius",  (float)radius);
    m_pointShader->SetUniform("color",  color  ) ;


    VBO::DrawIndices(*m_sphereVertexBuffer, *m_sphereIndexBuffer, GL_TRIANGLES, (m_sphereNumTriangles)*3);

}

/*
  Plan:

  send out normal as usually.

  send of textcoord distort
 */

void LightingPass::UpdateTextures(int lightCount) {

    if(lightCount == -1) {
	lightCount = GetTestLights().size();
    }

    // delete old textures.
    if(m_lightIndexTexture != NULL) {
	MY_DELETE(m_lightIndexTexture);
	MY_DELETE(m_lightIndexTextureBuffer);

    }

    int maxLightIndices = GRID_COUNT * GRID_COUNT * lightCount;

    /*
      Find smallest power-of-two texture to fit all the indies.
    */
    int texSize = 2;

    while(texSize * texSize < maxLightIndices) {
	texSize *= 2;
    }

    m_lightIndexTexture = new Texture2D(nullptr, texSize, texSize,
				       GL_R16F,
				       GL_RED,
				       GL_FLOAT);

    m_lightIndexTexture->Bind();
    m_lightIndexTexture->SetTextureClamping();
    m_lightIndexTexture->SetMinFilter(GL_NEAREST);
    m_lightIndexTexture->SetMagFilter(GL_NEAREST);
    m_lightIndexTexture->Unbind();

    m_lightIndexTextureBuffer = new float[texSize*texSize];
}
