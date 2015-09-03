#pragma once

#include "math/matrix4f.hpp"

#include "geometry_object_data.hpp"

class Vector3f;
class VBO;

class GeometryObject {

private:

/*    Matrix4f translationMatrix;
    Matrix4f scaleMatrix;
*/

    /*

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;
    */


public:

        VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    GLuint m_numTriangles;


//    GeometryObject(const Vector3f& translation, const Vector3f& scale);

//    GeometryObject(const Vector3f& translation, const Vector3f& scale);

    void Init(GeometryObjectData&  data);

    GeometryObject() {}
    ~GeometryObject();

    void Render();


//    Matrix4f GetModelMatrix()const;



};
