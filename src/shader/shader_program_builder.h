#pragma once

#include "../precomp.h"

class ShaderProgramBuilder {

private:

    void attach();
    void link();

    GLuint compiledFragmentShader;
    GLuint compiledVertexShader;

    GLuint shaderProgram;

    GLuint buildAndCompileShader(const std::string& shaderPath, GLenum shaderType);

    void bindAttribLocation(GLuint attribIndex, const std::string& attribName);

public:

    ShaderProgramBuilder(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);


    inline int getLinkedShaderProgram() {
	return this->shaderProgram;
    }
};
