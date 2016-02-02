#include "env_fbo.hpp"

#include "cube_map_texture.hpp"
#include "render_buffer.hpp"

#include "log.hpp"

void EnvFBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
/*
    width /= 2 * 2;
    height /= 2 * 2;


*/

    LOG_I("create env fbo");

    m_size = width;

/*
	m_envMap = CubeMapTexture::Load(m_size);

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_envMap->Bind();
	{
	    m_envMap->SetMagMinFilters(GL_LINEAR);
	    m_envMap->SetTextureClamping();
	}
	m_envMap->Unbind();
*/

    Bind();
    {
	m_envMap = CubeMapTexture::Load(m_size);

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_envMap->Bind();
	{
	    m_envMap->SetMagMinFilters(GL_LINEAR);
	    m_envMap->SetTextureClamping();
	}
	m_envMap->Unbind();

	// next we create a depth buffer, and attach it to the FBO.

	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorage(GL_DEPTH_COMPONENT, m_size, m_size);

	    // attach the depth buffer to the FBO.
	    glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle());
	}
	m_depthBuffer->Unbind();

	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();

}

size_t EnvFBO::GetSize() {
    return m_size;
}

CubeMapTexture* EnvFBO::GetEnvMap() {
    return m_envMap;
}



int EnvFBO::BindFace(int i) {

    Attach(
	GL_COLOR_ATTACHMENT0, m_envMap->GetHandle(),
	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, i, box2CubeMap, 0);

}
