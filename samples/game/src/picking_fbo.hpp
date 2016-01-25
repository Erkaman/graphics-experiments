#pragma once

#include "fbo.hpp"

struct PixelInfo {

public:
        float id;
        float unused1;
        float unused2;
};

class PickingFBO : public FBO{

public:

    PickingFBO();

    virtual ~PickingFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    PixelInfo ReadPixel(unsigned int x, unsigned int y);
};
