#pragma once

#include "geometry_object_data.hpp"

class EobFile {
public:

    static void Write(const GeometryObjectData& data, const std::string& outfile);

    static GeometryObjectData Read(const std::string& outfile);

};
