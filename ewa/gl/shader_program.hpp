#pragma once

#include <vector>
#include <string>
#include "gl_common.hpp"

class Color;
class Matrix4f;
class Vector4f;
class Vector3f;
class Vector2f;
class ICamera;
class UniformLocationStore;
class Paraboloid;

class ShaderProgram {

private:

    ShaderProgram(const ShaderProgram&);
    ShaderProgram& operator=(const ShaderProgram&);

    UniformLocationStore* m_uniformLocationStore;

    std::string m_shaderProgramName;

    GLuint m_shaderProgram;

    void CompileShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource,const std::string& geometryShaderSource, const std::string& path, void (*beforeLinkingHook)(GLuint));

    ShaderProgram();

public:

    static ShaderProgram* Load(const std::string& shaderName, void (*beforeLinkingHook)(GLuint) = NULL );

    // create shader from source code.
    static ShaderProgram* Load(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);



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


    void SetParaboloidUniforms(
    const Matrix4f& modelMatrix, const Matrix4f& viewMatrix,
    const Matrix4f& projectionMatrix, const Vector3f&
    cameraPosition, const Vector4f& lightDirection,
    const Paraboloid& paraboloid
	);


    void SetUniform(const std::string& uniformName, Vector3f* v, int size);
    void SetUniform(const std::string& uniformName, float* v, int size);


    void SetPhongUniforms(const Matrix4f& modelMatrix, const ICamera* camera, const Vector4f& lightDirection);


    void SetPhongUniforms(const Matrix4f& modelMatrix, const Matrix4f& viewMatrix,
			  const Matrix4f& projectionMatrix, const Vector3f& cameraPosition, const Vector4f& lightDirection);


    void SetPhongUniforms(
	const Matrix4f& modelMatrix, const ICamera* camera, const Vector4f& lightDirection, const Matrix4f& lightVp);

    void SetLightUniforms(
	const ICamera* camera, const Vector4f& lightDirection, const Matrix4f& lightVp);

    void SetShaderUniforms(const Matrix4f& modelMatrix, const ICamera* camera);
    void SetShaderUniforms(const Matrix4f& modelMatrix, const Matrix4f& viewMatrix,
	const Matrix4f& projectionMatrix);


    void SetMvpUniform(const Matrix4f& mvp);


    void Query();

};
