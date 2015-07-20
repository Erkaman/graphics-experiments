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

    GLuint BuildAndCompileShader(const std::string& shaderSource, const GLenum shaderType, const std::string& path);
    void BindAttribLocation(const GLuint attribIndex, const std::string& attribName);
    void Attach();
    void Link();

public:

    /*
      The source code of the shaders are passed as arguments.
     */
    ShaderProgramBuilder(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& geometryShaderSource, const std::string& path);

    GLuint GetLinkedShaderProgram();
};
