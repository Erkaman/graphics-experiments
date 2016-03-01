#pragma once

#include "fbo.hpp"

class PositionFbo : public FBO{

public:

    PositionFbo();

    virtual ~PositionFbo() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);



};
