#pragma once

#include "math/matrix4f.hpp"

#include "geometry_object_data.hpp"

class Vector3f;
class VBO;

class GeometryObject {
private:

    struct Chunk {
	VBO* m_vertexBuffer;
	VBO* m_indexBuffer;
	GLuint m_numTriangles;
    };

    std::vector<Chunk*> m_chunks;

public:


    void Init(GeometryObjectData&  data);

    GeometryObject() {}
    ~GeometryObject();

    void Render();
};
