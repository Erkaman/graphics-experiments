#pragma once

#include "fbo.hpp"

enum GBUFFER_TEXTURE_UNITS {

    COLOR_TEXTURE_UNIT,
    DEPTH_TEXTURE_UNIT,
    NORMAL_TEXTURE_UNIT,
    POSITION_TEXTURE_UNIT,
};

class Gbuffer : public FBO{

private:

    Texture* m_colorTexture;
    Texture* m_depthTexture;
    Texture* m_normalTexture;
    Texture* m_positionTexture;

public:

    Gbuffer();

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    virtual ~Gbuffer() {}

    Texture* GetColorTexture();
    Texture* GetDepthTexture();
    Texture* GetNormalTexture();
    Texture* GetPositionTexture();
};
