#include "font.hpp"

#include "gl/texture2d.hpp"
#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"

#include "math/color.hpp"


using std::vector;
using std::string;

GLfloat vertices[2* 4 * 2];


Font::~Font() {
    delete m_fontTexture;
    delete m_vertexBuffer;
}

Font::Font(
    const std::string& fontTexturePath,
    const unsigned int fontTextureWidth, const unsigned int fontTextureHeight,
    const unsigned int fontCellWidth, const unsigned int fontCellHeight,
    const unsigned int windowWidth, const unsigned int windowHeight):
    m_fontTextureWidth(fontTextureWidth), m_fontTextureHeight(fontTextureHeight),
    m_fontCellWidth(fontCellWidth), m_fontCellHeight(fontCellHeight),

    m_scaleX(2.0f / windowWidth), m_scaleY(2.0f / windowHeight) {


    m_fontTexture = new Texture2D(fontTexturePath);
    m_fontTexture->Bind();
    m_fontTexture->SetTextureClamping();
    m_fontTexture->SetMagMinFilters(GL_LINEAR);
    m_fontTexture->Unbind();


    // allocate vbo.

    m_vertexBuffer = VBO::CreateInterleaved(
					      vector<GLuint>{
						  VBO_POSITION_ATTRIB_INDEX,
						      VBO_TEX_COORD_ATTRIB_INDEX},
					      vector<GLuint>{2,2},
					      GL_DYNAMIC_DRAW
					      );


    float x = 0;
    float y = 0;

    // create the font table.
    for(size_t i = 0; i < 95; ++i) {
	fontTable.emplace_back(x,y);

	x += m_fontCellWidth;

	if(x >= fontTextureWidth) {
	    x = 0;
	    y += fontCellHeight;
	}

    }
}

void Font::DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str, const Color& color) {

    // scale the coordinate to the window.
    float scaledX = -1 + x * m_scaleX;
    float scaledY = +1 - y* m_scaleY;


    float totalWidth = 0;
    float totalHeight = 0;

    Texture::SetActiveTextureUnit(0);
    fontShader.SetUniform("tex", 0);

    m_fontTexture->Bind();
    {

	// currently not working.
//	fontShader.SetUniform("color", Color(1,0,0));

	const float scaledCharacterWidth = 0.5f * m_fontCellWidth * m_scaleX; // TODO: is this really correct?
	const float scaledCharacterHeight = 0.5f * m_fontCellHeight * m_scaleY;

	for(const char ch : str) {

	    if(ch == '\n') {
		// go to next line, but draw nothing.
		totalHeight -= scaledCharacterHeight;
		totalWidth = 0;
		continue;
	    }


	    const Vector2f position = LookupChar(ch);

	    DrawQuad((scaledX + totalWidth), scaledY+ totalHeight,
		     (scaledX + totalWidth + scaledCharacterWidth), (scaledY + scaledCharacterHeight + totalHeight),

		     position.x,position.y, position.x + m_fontCellWidth,
		     position.y + m_fontCellHeight
		);

	    totalWidth += scaledCharacterWidth;

	}

    }
    m_fontTexture->Unbind();


}

void Font::DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str) {
    DrawString(fontShader, x, y, str, Color::White());
}

Vector2f Font::LookupChar(const char ch) {
    assert(ch >= 32 && ch <= 126);

    Vector2f position = fontTable[ch - 32];

    return position;
}


void Font::DrawQuad(const float drawX, const float drawY, const float drawX2, const float drawY2,
		    const float srcX, const float srcY, const float srcX2, const float srcY2) {
    const float drawWidth = (drawX2 - drawX);
    const float drawHeight = (drawY2 - drawY);
    const float textureSrcX = srcX / m_fontTextureWidth;
    const float textureSrcY = srcY / m_fontTextureHeight;
    const float srcWidth = srcX2 - srcX;
    const float srcHeight = srcY2 - srcY;
    const float renderWidth = (srcWidth / m_fontTextureWidth);
    const float renderHeight = (srcHeight / m_fontTextureHeight);

    int i = 0;


    vertices[i++] =(drawX);
    vertices[i++] =(drawY);
    vertices[i++] =(textureSrcX);
    vertices[i++] =(textureSrcY + renderHeight);

    vertices[i++] =(drawX + drawWidth);
    vertices[i++] =(drawY);
    vertices[i++] =(textureSrcX + renderWidth);
    vertices[i++] =(textureSrcY + renderHeight);

    vertices[i++] =(drawX);
    vertices[i++] =(drawY + drawHeight);
    vertices[i++] =(textureSrcX);
    vertices[i++] =(textureSrcY);

    vertices[i++] =(drawX + drawWidth);
    vertices[i++] =(drawY + drawHeight);
    vertices[i++] =(textureSrcX + renderWidth);
    vertices[i++] =(textureSrcY);


    m_vertexBuffer->Bind();
    m_vertexBuffer->SetBufferData(2* 4 * 2 * sizeof(GLfloat), vertices);
    m_vertexBuffer->Unbind();

    m_vertexBuffer->EnableVertexAttribInterleavedWithBind();
    m_vertexBuffer->DrawVertices(GL_TRIANGLE_STRIP,4);
    m_vertexBuffer->DisableVertexAttribInterleavedWithBind();
}
