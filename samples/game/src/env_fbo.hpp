#pragma once

#include "ewa/gl/fbo.hpp"

class EnvFBO : public FBO{

public:

    EnvFBO() {}

    virtual ~EnvFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);
};
