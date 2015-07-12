#pragma once

#include "gl_common.hpp"
#include <string>

class ShaderProgramBuilder {

private:


    GLuint m_compiledFragmentShader;
    GLuint m_compiledVertexShader;
    GLuint m_compiledGeometryShader;

    bool m_hasGeometryShader;

    GLuint m_shaderProgram;

    GLuint BuildAndCompileShader(const std::string& shaderPath, const GLenum shaderType);
    void BindAttribLocation(const GLuint attribIndex, const std::string& attribName);
    void Attach();
    void Link();


public:

    ShaderProgramBuilder(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath);

    GLuint GetLinkedShaderProgram();
};
