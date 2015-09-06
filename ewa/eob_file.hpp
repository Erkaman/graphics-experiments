#pragma once

#include "geometry_object_data.hpp"

class EobFile {
public:

    static void Write(const GeometryObjectData& data, const std::string& outfile);

    // TODO: clean up the memory allocated in this method.
    static GeometryObjectData Read(const std::string& infile);

};
