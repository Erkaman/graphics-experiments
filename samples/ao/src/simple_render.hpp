#pragma once

#include "ewa/gl/gl_common.hpp"
#include "ewa/math/vector3f.hpp"

class GeometryObjectData;
class VBO;
class ShaderProgram;
class ICamera;
class ArrayTexture;
class Chunk;

class SimpleRender {

private:

    GeometryObjectData* m_eob;

    VBO* m_vertexBuffer;

    VBO* m_indexBuffer;
    GLuint m_numTriangles;

    float m_shininess;
    Vector3f m_specularColor;
    Vector3f m_diffuseColor;

    // the material.
    GLint m_texture;
    GLint m_normalMap;
    GLint m_specularMap;

    bool m_hasHeightMap;

    ShaderProgram* m_simpleShader;
    ShaderProgram* m_aoShader;

    ArrayTexture* m_arrayTexture;

    std::vector<Chunk*> m_chunks;

    bool isAo;
public:

    SimpleRender();

    void SetEob(GeometryObjectData* eob, const std::string& basePath);

    void Render(ICamera* camera, const Vector4f& lightPosition);

    void Save();

};
