#pragma once

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

class GeometryObjectData;



class RayTracer {

private:
    GeometryObjectData* m_geoObj;



public:
    RayTracer(GeometryObjectData* geoObj);


    GeometryObjectData* RayTrace();

};
