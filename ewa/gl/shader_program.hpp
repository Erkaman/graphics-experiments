#pragma once

#include <vector>
#include <string>
#include "gl_common.hpp"

class Color;
class Matrix4f;
class Vector4f;
class Vector3f;
class Vector2f;
class Camera;
class UniformLocationStore;

class ShaderProgram {

private:

    ShaderProgram(const ShaderProgram&);
    ShaderProgram& operator=(const ShaderProgram&);

    UniformLocationStore* m_uniformLocationStore;

    std::vector<std::string> m_warnedUniforms;

    std::string m_shaderProgramName;

    GLuint m_shaderProgram;

    void CompileShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource,const std::string& geometryShaderSource, const std::string& path, void (*beforeLinkingHook)(GLuint));

    ShaderProgram();

    void SetUniformWarn(const std::string& uniformName);

public:

    ShaderProgram(const std::string& shaderName, void (*beforeLinkingHook)(GLuint) = NULL );

    // create shader from source code.
    ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

    ~ShaderProgram();



    void Bind();

    void Unbind();

    void SetUniform(const std::string& uniformName, const Color& color);
    void SetUniform(const std::string& uniformName, const Matrix4f& matrix);
    void SetUniform(const std::string& uniformName, const int val);
    void SetUniform(const std::string& uniformName, const float val);
    void SetUniform(const std::string& uniformName, const Vector4f& v);
    void SetUniform(const std::string& uniformName, const Vector3f& v);
    void SetUniform(const std::string& uniformName, const Vector2f& v);

    void SetPhongUniforms(const Matrix4f& modelMatrix, const Camera& camera, const Vector4f& lightPosition);

    void Query();

};
