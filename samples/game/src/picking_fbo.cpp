#include "picking_fbo.hpp"

#include "gl/texture.hpp"
#include "gl/render_buffer.hpp"

#include "log.hpp"

void PickingFBO::RecreateBuffers(const GLsizei width, const GLsizei height)  {
    Bind();
    {
	// first we create a render target, and attach it the FBO.
	m_renderTargetTexture = new Texture(
	    GL_TEXTURE_2D,
	    width,
	    height,
	    GL_RGB32F, // internal format
	    GL_RGB, // format
	    GL_FLOAT); // type

	Texture::SetActiveTextureUnit(m_targetTextureUnit);
	m_renderTargetTexture->Bind();
	{
	    m_renderTargetTexture->SetMagMinFilters(GL_NEAREST);
	    m_renderTargetTexture->SetTextureClamping();

	    // attach the target texture to the FBO.
	    Attach(GL_COLOR_ATTACHMENT0, *m_renderTargetTexture);

	}
	m_renderTargetTexture->Unbind();

	// next we create a depth buffer, and attach it to the FBO.

	m_depthBuffer = new RenderBuffer();
	m_depthBuffer->Bind();
	{
	    m_depthBuffer->RenderbufferStorage(GL_DEPTH_COMPONENT, width, height);

	    // attach the depth buffer to the FBO.
	    GL_C(glFramebufferRenderbuffer(m_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_depthBuffer->GetHandle()));
	}
	m_depthBuffer->Unbind();


	GL_C(glReadBuffer(GL_NONE));
	GL_C(glDrawBuffer(GL_COLOR_ATTACHMENT0));

	CheckFramebufferStatus();

    }
    // switch to default frame buffer.
    Unbind();
}

PickingFBO::PickingFBO() {

}


PixelInfo PickingFBO::ReadPixel(unsigned int x, unsigned int y) {
    BindForReading();
    GL_C(glReadBuffer(GL_COLOR_ATTACHMENT0));

    LOG_I("read pixel: %d, %d", x, y);

     PixelInfo pixel;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);


    GL_C(glReadBuffer(GL_NONE));
    UnbindForReading();

    return pixel;

}

//the solution is to try and write it to a file using glReadPixels.
