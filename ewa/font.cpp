#include "font.hpp"

#include "gl/texture2d.hpp"
#include "gl/vbo.hpp"
#include "gl/shader_program.hpp"

#include "math/vector3f.hpp"

#include "file.hpp"
#include "string_util.hpp"
#include "resource_manager.hpp"
#include "buffered_file_reader.hpp"

#define START_CHAR 32
#define END_CHAR 126


using std::vector;
using std::string;

GLfloat vertices[2* 4 * 2];


Font::~Font() {
    delete m_fontTexture;
    delete m_vertexBuffer;
}

Font::Font(const unsigned int windowWidth, const unsigned int windowHeight,
     const float fontScale):
    m_windowScaleX(2.0f / windowWidth), m_windowScaleY(2.0f / windowHeight),
    atlasTable(END_CHAR-START_CHAR+1), m_fontScale(fontScale) {}


Font* Font::Load(
    const std::string& atlasPath,
    const std::string& amfPath,
    const unsigned int windowWidth, const unsigned int windowHeight,
    const float fontScale
    )
{

    Font* font = new Font(windowWidth, windowHeight, fontScale);

    font->m_fontTexture = Texture2D::Load(atlasPath);
    font->m_fontTexture->Bind();
    font->m_fontTexture->SetTextureClamping();
    font->m_fontTexture->SetMagMinFilters(GL_LINEAR);
    font->m_fontTexture->Unbind();

    font->m_atlasWidth = font->m_fontTexture->GetWidth();
    font->m_atlasHeight = font->m_fontTexture->GetHeight();

    // The .amf format is described in the README of this repo:
    // https://github.com/Erkaman/font_creator_cpp
   // File* atlasFile = ResourceManager::OpenResourceForReading(amfPath);
	BufferedFileReader* atlasFile = BufferedFileReader::Load(amfPath);

    if(!atlasFile) {
	return NULL;
    }

    
    while(!atlasFile->IsEof()){

	string str = atlasFile->ReadLine();

	// first char is always the character.
	char ch = str[0];

	vector<string> tokens = StringUtil::SplitString(str.substr(2), ",");



	if (tokens.size() != 5) {

		SetError("Error parsing amf file %s: found line with non-five tokens: %s", amfPath.c_str(), str.c_str() );
		return NULL;
	}

	// the character of the token.


	AtlasEntry entry;
	entry.x = stoi(tokens[0]);
	entry.y = stoi(tokens[1]);
	entry.xSize = stoi(tokens[2]);
	entry.ySize = stoi(tokens[3]);
	entry.preAdvance = stoi(tokens[4]);

        font->atlasTable[ch-START_CHAR] = entry;

    }

    // allocate vbo.

    font->m_vertexBuffer = VBO::CreateInterleaved(
					      vector<GLuint>{2,2}, // position, tex
					      GL_DYNAMIC_DRAW
					      );

    return font;


}

void Font::DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str, const Vector3f& color) {

    // scale the coordinate to the window.
    float scaledX = -1 + x * m_windowScaleX;
    float scaledY = +1 - y* m_windowScaleY;


    float totalWidth = 0;
    float totalHeight = 0;

    Texture::SetActiveTextureUnit(0);
    fontShader.SetUniform("font_tex", 0);

    m_fontTexture->Bind();
    {

	// set the color.
	fontShader.SetUniform("color", color);

	for(const char ch : str) {

	    const Font::AtlasEntry entry = LookupChar(ch);



	    // 1.0f is real size.
	    const float scaledCharacterWidth = m_fontScale * (entry.xSize) * m_windowScaleX; // TODO: is this really correct?
	    const float scaledCharacterHeight = m_fontScale * entry.ySize * m_windowScaleY;

	    if(ch == '\n') {
		// go to next line, but draw nothing.
		totalHeight -= scaledCharacterHeight;
		totalWidth = 0;
		continue;
	    }

	    DrawQuad((scaledX + totalWidth), scaledY+ totalHeight,
		     (scaledX + totalWidth + scaledCharacterWidth), (scaledY + scaledCharacterHeight + totalHeight),

		     entry.x,entry.y, entry.x + entry.xSize,
		     entry.y + entry.ySize
		);

	    totalWidth += scaledCharacterWidth;

	}

    }
    m_fontTexture->Unbind();
}

void Font::DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str) {
    DrawString(fontShader, x, y, str, Vector3f(1,1,1));
}

Font::AtlasEntry Font::LookupChar(const char ch) {
    assert(ch >= START_CHAR && ch <= END_CHAR);

    return atlasTable[ch - START_CHAR];
}


void Font::DrawQuad(const float drawX, const float drawY, const float drawX2, const float drawY2,
		    const float srcX, const float srcY, const float srcX2, const float srcY2) {
    const float drawWidth = (drawX2 - drawX);
    const float drawHeight = (drawY2 - drawY);
    const float textureSrcX = srcX / m_atlasWidth;
    const float textureSrcY = srcY / m_atlasHeight;
    const float srcWidth = srcX2 - srcX;
    const float srcHeight = srcY2 - srcY;
    const float renderWidth = (srcWidth / m_atlasWidth);
    const float renderHeight = (srcHeight / m_atlasHeight);

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
