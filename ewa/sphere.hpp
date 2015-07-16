#pragma once

class VBO;
class ShaderProgram;
class Camera;
class Vector4f;

class Sphere {

private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;


    ShaderProgram* m_shader;

public:

    Sphere();
    ~Sphere();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

};
