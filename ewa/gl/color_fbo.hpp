#pragma once

#include "fbo.hpp"

class ColorFBO : public FBO{

public:

    ColorFBO();


    virtual ~ColorFBO() {}


    void RecreateBuffers(const GLsizei width, const GLsizei height);



};
