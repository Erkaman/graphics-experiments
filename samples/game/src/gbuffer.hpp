#pragma once

#include "fbo.hpp"

enum GBUFFER_TEXTURE_UNITS {

    COLOR_TEXTURE_UNIT,
    DEPTH_TEXTURE_UNIT,
    NORMAL_TEXTURE_UNIT,
    SPECULAR_TEXTURE_UNIT,
    SHADOW_TEXTURE_UNIT,
    ENV_TEXTURE_UNIT,
    REFRACTION_TEXTURE_UNIT,
    REFLECTION_TEXTURE_UNIT,
    LIGHT_GRID_TEXTURE_UNIT,
    LIGHT_INDEX_TEXTURE_UNIT,

};

class Gbuffer : public FBO{

private:

    Texture* m_colorTexture;
    Texture* m_depthTexture;
    Texture* m_normalTexture;

public:

    Gbuffer();

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    virtual ~Gbuffer() {}

    Texture* GetColorTexture();
    Texture* GetDepthTexture();
    Texture* GetNormalTexture();
};
