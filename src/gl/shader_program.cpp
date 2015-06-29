#include "shader_program.hpp"

#include "uniform_location_store.hpp"
#include "shader_program_builder.hpp"

#include "common.hpp"

#include "math/color.hpp"
#include "math/matrix4f.hpp"
#include "math/vector4f.hpp"
#include "math/vector3f.hpp"


using namespace std;

ShaderProgram::ShaderProgram():m_uniformLocationStore(nullptr) {

}

ShaderProgram::ShaderProgram(const std::string& shaderName){

    m_alreadyBoundProgram = false;

    CompileShaderProgram(shaderName + "_vs.glsl", shaderName + "_fs.glsl");
}

void ShaderProgram::CompileShaderProgram(const string& vertexShaderPath, const string& fragmentShaderPath) {
    // link shader program.
    ShaderProgramBuilder shaderBuilder(vertexShaderPath, fragmentShaderPath);
    m_shaderProgram = shaderBuilder.getLinkedShaderProgram();

    m_uniformLocationStore = make_unique<UniformLocationStore>(m_shaderProgram);
}

void ShaderProgram::Bind() {
    if(m_alreadyBoundProgram)
	return;

    GL_C(glUseProgram(m_shaderProgram));

    m_alreadyBoundProgram = true;
}

void ShaderProgram::Unbind() {
    if(!m_shaderProgram) {
	return;
    }

    GL_C(glUseProgram(0));
    m_alreadyBoundProgram = false;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_alreadyBoundProgram);
}

void UniformLocationStoreDeleter::operator()(UniformLocationStore *p)
{
    delete p;
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
