#include "color_depth_fbo.hpp"

#include "gl/texture.hpp"

void ColorDepthFbo::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_colorTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGBA8, // internal format
	    GL_RGBA, // format
	    GL_UNSIGNED_BYTE); // type

	Texture::SetActiveTextureUnit(0);
	m_colorTexture->Bind();
	{
	    m_colorTexture->SetMagMinFilters(GL_LINEAR);
	    m_colorTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT0, *m_colorTexture);

	}
	m_colorTexture->Unbind();





	// first we create a render target, and attach it the FBO.
	m_depthTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_DEPTH_COMPONENT32, // internal format
	    GL_DEPTH_COMPONENT,  // format
	    GL_UNSIGNED_INT); // type

	Texture::SetActiveTextureUnit(1);
	m_depthTexture->Bind();
	{
	    m_depthTexture->SetMagMinFilters(GL_NEAREST);
	    m_depthTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_DEPTH_ATTACHMENT, *m_depthTexture);
	}
	m_depthTexture->Unbind();

	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();
}

ColorDepthFbo::ColorDepthFbo() {

}

Texture* ColorDepthFbo::GetColorTexture() {
    return m_colorTexture;
}

Texture* ColorDepthFbo::GetDepthTexture() {
    return m_depthTexture;
}
