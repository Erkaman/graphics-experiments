#include "shader_program.hpp"

#include "uniform_location_store.hpp"
#include "shader_program_builder.hpp"

using namespace std;

ShaderProgram::~ShaderProgram() {}

ShaderProgram::ShaderProgram() {
    uniformLocationStore = nullptr;
}

ShaderProgram::ShaderProgram(const std::string& shaderName){

    this->alreadyBoundProgram = false;

    CompileShaderProgram(shaderName + "_vs.glsl", shaderName + "_fs.glsl");
}

void ShaderProgram::CompileShaderProgram(const string& vertexShaderPath, const string& fragmentShaderPath) {
    // link shader program.
    ShaderProgramBuilder shaderBuilder(vertexShaderPath, fragmentShaderPath);
    shaderProgram = shaderBuilder.getLinkedShaderProgram();

    uniformLocationStore = make_unique<UniformLocationStore>(shaderProgram);
}


inline void ShaderProgram::Bind() {
    if(alreadyBoundProgram)
	return;

    glUseProgram(shaderProgram);

    alreadyBoundProgram = true;
}

inline void ShaderProgram::Unbind() {
    if(!shaderProgram) {
	return;
    }

    glUseProgram(0);
    alreadyBoundProgram = false;
}

inline void ShaderProgram::Dispose() {
    glDeleteProgram(alreadyBoundProgram);
}

void UniformLocationStoreDeleter::operator()(UniformLocationStore *p)
{
    delete p;
}
