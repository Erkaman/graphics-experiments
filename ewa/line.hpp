#pragma once

#include "ewa/gl/vbo.hpp"

#include "ewa/math/vector3f.hpp"

class ShaderProgram;
class Matrix4f;

// class for drawing lines.
class Line {

private:

    VBO* m_vertexBuffer;

    // line start point.
    Vector3f m_start;

    // line end point.
    Vector3f m_end;

    Vector3f m_color;

    ShaderProgram* m_shader;


    Line(const Vector3f& start, const Vector3f& end, const Vector3f& color, ShaderProgram* shader);

public:

    static Line* Load(const Vector3f& start, const Vector3f& end, const Vector3f& color = Vector3f(1.0f,1.0f,0.0f) );
    ~Line();

    void SetStartEnd(const Vector3f& start, const Vector3f& end);
    void SetStart(const Vector3f& start);
    void SetEnd(const Vector3f& end);
    void SetColor(const Vector3f& color);

    void Render(const Matrix4f& vp);
};
