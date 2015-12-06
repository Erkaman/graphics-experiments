#pragma once

#include "ewa/gl/vbo.hpp"

#include "ewa/math/vector3f.hpp"

class ShaderProgram;
class Matrix4f;

// class for drawing a collection of points.
class Points {

private:

    VBO* m_vertexBuffer;

    std::vector<Vector3f> m_points;

    Vector3f m_color;
    GLfloat m_pointSize;

    ShaderProgram* m_shader;

    Points(
	std::vector<Vector3f> points, const GLfloat pointSize, const Vector3f& color, ShaderProgram* shader );

public:

    static Points* Load(
	std::vector<Vector3f> points, const GLfloat pointSize = 7.0, const Vector3f& color = Vector3f(1.0f,1.0f,0.0f) );

    ~Points();

    void SetPoints(std::vector<Vector3f> points);

    void Render(const Matrix4f& vp);
};
