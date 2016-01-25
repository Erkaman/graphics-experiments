#pragma once

#include "fbo.hpp"

class Gbuffer : public FBO{

private:

    Texture* m_colorTexture;


public:

    Gbuffer();

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    virtual ~Gbuffer() {}

    Texture* GetColorTexture();

};
