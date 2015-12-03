#include "depth_fbo.hpp"

#include "gl/texture.hpp"
#include "gl/render_buffer.hpp"


void DepthFBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
   Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_renderTargetTexture = new Texture(

	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_DEPTH_COMPONENT16,// internal format
	    GL_DEPTH_COMPONENT, // format
	    GL_FLOAT); // type

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_renderTargetTexture->Bind();
	{
	    m_renderTargetTexture->SetMagMinFilters(GL_LINEAR);
	    m_renderTargetTexture->SetTextureClamping();
	    m_renderTargetTexture->ConfigureForPCF();

	    // attach the target texture to the FBO.
	    Attach(GL_DEPTH_ATTACHMENT, *m_renderTargetTexture);

	    GL_C(glDrawBuffer(GL_NONE));  // we are only interested in the depth. output no color.

	}
	m_renderTargetTexture->Unbind();


	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();
}

DepthFBO::DepthFBO() {
}
