#pragma once

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

#include "ewa/gl/gl_common.hpp"


class GeometryObjectData;
class PositionFbo;
class VBO;
class ShaderProgram;

struct Chunk {
    VBO* m_indexBuffer;
    GLuint m_numTriangles;
};

class RayTracer {

private:
    GeometryObjectData* m_geoObj;

    PositionFbo* m_positionFbo;

    VBO* m_vertexBuffer;

    std::vector<Chunk> m_chunks;

    ShaderProgram* m_shader;


public:
    RayTracer(GeometryObjectData* geoObj);


    GeometryObjectData* RayTrace();

};
