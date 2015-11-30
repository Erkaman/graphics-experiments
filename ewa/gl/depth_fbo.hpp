#pragma once

#include "fbo.hpp"

class DepthFBO : public FBO{

public:

    DepthFBO();

    virtual ~DepthFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);


};
