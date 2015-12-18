#pragma once

#include "fbo.hpp"


class PickingFBO : public FBO{

public:

    PickingFBO();

    virtual ~PickingFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);
};
