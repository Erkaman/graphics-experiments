#pragma once

#include "fbo.hpp"

// used for shadow mapping
class DepthFBO : public FBO{

public:

    DepthFBO();

    virtual ~DepthFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);


};
