#pragma once

#include <vector>
#include <string>
#include <memory>
#include "gl_common.hpp"

class Color;
class Matrix4f;
class Vector4f;

class UniformLocationStore;
struct UniformLocationStoreDeleter
{
    void operator()(UniformLocationStore *p);
};

class ShaderProgram {

private:

    std::unique_ptr<UniformLocationStore> m_uniformLocationStore;

    std::vector<std::string> m_warnedUniforms;

    std::string m_shaderProgramName;

    bool m_alreadyBoundProgram;

    GLuint m_shaderProgram;

    void CompileShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    ShaderProgram();

    void SetUniformWarn(const std::string& uniformName);

public:

    ShaderProgram(const std::string& shaderName);
    ~ShaderProgram();

    void Bind();

    void Unbind();

    void Dispose();

    void SetUniform(const std::string& uniformName, const Color& color);
    void SetUniform(const std::string& uniformName, const Matrix4f& matrix);
    void SetUniform(const std::string& uniformName, const int val);
    void SetUniform(const std::string& uniformName, const Vector4f& v);

};
