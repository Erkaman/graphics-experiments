#include "shader_program.hpp"

#include "uniform_location_store.hpp"
#include "shader_program_builder.hpp"
#include "resource_manager.hpp"

#include "icamera.hpp"
#include "log.hpp"
#include "string_util.hpp"
#include "common.hpp"

#include "math/color.hpp"
#include "math/matrix4f.hpp"
#include "math/vector4f.hpp"
#include "math/vector3f.hpp"
#include "math/vector2f.hpp"

#include "file.hpp"


using namespace std;

ShaderProgram::ShaderProgram():m_uniformLocationStore(nullptr) {

}

ShaderProgram::~ShaderProgram() {
    MY_DELETE(m_uniformLocationStore);

    glDeleteProgram(m_shaderProgram);
}

ShaderProgram* ShaderProgram::Load(const std::string& shaderName, void (*beforeLinkingHook)(GLuint) ) {

    string geometryShaderPath = shaderName + "_gs.glsl";

    string geometryShaderSource;

    if(	!ResourceManager::GetInstance().ResourceExists(geometryShaderPath)) {
	geometryShaderSource = ""; // do not load a geometry shader, because it does not exist.
    } else {
	geometryShaderSource = ResourceManager::LocateAndReadResource(geometryShaderPath);
    }

    ShaderProgram* shaderProgram = new ShaderProgram();
    shaderProgram->CompileShaderProgram(

	ResourceManager::LocateAndReadResource(shaderName + "_vs.glsl"),
	ResourceManager::LocateAndReadResource(shaderName + "_fs.glsl"),
	geometryShaderSource,
	File::GetFilePath(shaderName + "_vs.glsl"),
	beforeLinkingHook);

    return shaderProgram;


}

ShaderProgram* ShaderProgram::Load(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    ShaderProgram* shaderProgram = new ShaderProgram();

    shaderProgram->CompileShaderProgram(

	vertexShaderSource,
	fragmentShaderSource,
	"",
	"", NULL);

    return shaderProgram;
}

void ShaderProgram::CompileShaderProgram(const string& vertexShaderSource, const string& fragmentShaderSource,const string& geometryShaderSource, const std::string& path, void (*beforeLinkingHook)(GLuint)) {
    // link shader program.

    ShaderProgramBuilder shaderBuilder(vertexShaderSource, fragmentShaderSource, geometryShaderSource, path, beforeLinkingHook);
    m_shaderProgram = shaderBuilder.GetLinkedShaderProgram();

    m_uniformLocationStore = new UniformLocationStore(m_shaderProgram);
}

void ShaderProgram::Bind() {
    GL_C(glUseProgram(m_shaderProgram));
}

void ShaderProgram::Unbind() {
    GL_C(glUseProgram(0));
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Color& color) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform4f(location, color.r, color.g, color.b, color.a));
    } else {
	SetUniformWarn(uniformName);
    }
}

void ShaderProgram::SetUniformWarn(const std::string& uniformName) {
    // we will only issue the warning once for every uniform. Otherwise it will completely spam the log.
    for(const std::string& warned : m_warnedUniforms) {
	if(warned == uniformName) {
	    return;
	}
    }

    LOG_W("Uniform %s could not be found(the GLSL compiler may have optimized it away)", uniformName.c_str() );
    m_warnedUniforms.push_back(uniformName);
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Matrix4f& matrix) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	const GLfloat* arr = reinterpret_cast<const GLfloat*>(&matrix);
	GL_C(glUniformMatrix4fv(location, 1, true, arr));
    } else {
	SetUniformWarn(uniformName);
    }
}


void ShaderProgram::SetUniform(const std::string& uniformName, const int val) {

    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform1i(location, val));
    } else {
	SetUniformWarn(uniformName);
    }
}


void ShaderProgram::SetUniform(const std::string& uniformName, const Vector4f& v) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform4f(location, v.x, v.y, v.z, v.w));
    } else {
	SetUniformWarn(uniformName);
    }
}

void ShaderProgram::SetUniform(const std::string& uniformName, const Vector3f& v) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform3f(location, v.x, v.y, v.z));
    } else {
	SetUniformWarn(uniformName);
    }
}

void ShaderProgram::SetPhongUniforms(const Matrix4f& modelMatrix, const ICamera* camera, const Vector4f& lightDirection, const Matrix4f& lightVp) {

    const Matrix4f modelViewMatrix = /*Matrix4f::CreateLookAt(
	-Vector3f(  Vector3f(-0.705072, -0.458142, -0.705072)),
	    Vector3f(0.0f),
	    Vector3f(0.0, 1.0, 0.0)
	) * modelMatrix;*/
	camera->GetViewMatrix() * modelMatrix;

    const Matrix4f mvp = camera->GetProjectionMatrix() * modelViewMatrix;
//	Matrix4f::CreateOrthographic(-20,20, -20, 20, -20, 20) * modelViewMatrix;

    SetUniform("mvp", mvp);
    SetUniform("lightMvp", lightVp * modelMatrix);


    SetUniform("modelViewMatrix", modelViewMatrix);
    SetUniform("normalMatrix", Matrix4f::GetNormalMatrix(modelViewMatrix));

    SetUniform("viewSpaceLightDirection", Vector3f(camera->GetViewMatrix() * (lightDirection)  ) );
    SetUniform("eyePos", camera->GetPosition() );

}

void ShaderProgram::SetUniform(const std::string& uniformName, const float val) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform1f(location, val));
    } else {
	SetUniformWarn(uniformName);
    }
}


void ShaderProgram::SetUniform(const std::string& uniformName, const Vector2f& v) {
    if (m_uniformLocationStore->UniformExists(uniformName)) {
	const GLuint location =m_uniformLocationStore->GetUniformLocation(uniformName);
	GL_C(glUniform2f(location, v.x, v.y));
    } else {
	SetUniformWarn(uniformName);
    }
}


std::string TypeToString(GLenum type) {

    switch(type) {
    case GL_FLOAT:
	return "GL_FLOAT";
    case GL_FLOAT_VEC3:
	return "GL_FLOAT_VEC3";
    default:
	return "Unknown";
    }
}

void ShaderProgram::Query() {

    GLint numVaryings;
    glGetProgramiv(m_shaderProgram, GL_TRANSFORM_FEEDBACK_VARYINGS, &numVaryings);



    for(int i = 0; i < numVaryings; ++i) {

	char nameBuffer[256];
	GLsizei length; // the actual size of the name.
	GLsizei size; // size of the varying.
	GLenum type; // type of the varying

	glGetTransformFeedbackVarying(m_shaderProgram, i,256, &length, &size, &type, nameBuffer);

	string nameStr = string(nameBuffer);

	LOG_I("name: %s", nameStr.c_str() );
	LOG_I("size: %d", size );
	LOG_I("type: %s", TypeToString(type).c_str() );

	LOG_I("\n" );

    }
}
