#pragma once

#include "fbo.hpp"

class ColorDepthFbo : public FBO{

    Texture* m_colorTexture;
    Texture* m_depthTexture;

public:

    ColorDepthFbo();

    virtual ~ColorDepthFbo() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    Texture* GetColorTexture();
    Texture* GetDepthTexture();
};
