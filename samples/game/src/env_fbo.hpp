#pragma once

#include "ewa/gl/fbo.hpp"

class CubeMapTexture;

class EnvFBO : public FBO{

private:

    CubeMapTexture* m_envMap;
    size_t m_size;

public:

    EnvFBO() {}

    virtual ~EnvFBO() {}

    void RecreateBuffers(const GLsizei width, const GLsizei height);

    size_t GetSize();
    CubeMapTexture* GetEnvMap();

    int BindFace(int i); // bind one of the six faces of the cubemap.



};
