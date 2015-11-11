#pragma once

#include <string>
#include <vector>


class Texture;
class VBO;
class ShaderProgram;
class Color;
class Vector3f;

class Font {

private:

    Texture* m_fontTexture;
    VBO* m_vertexBuffer;

    unsigned int m_atlasWidth;
    unsigned int m_atlasHeight;

    // Makes sure that the font properly scales for varying window proportions.
    const float m_windowScaleX;
    const float m_windowScaleY;

    // used to scale the size of the font.
    const float m_fontScale;

    struct AtlasEntry{
	unsigned int x;
	unsigned int y;
	unsigned int xSize;
	unsigned int ySize;
	signed int preAdvance;
    };

    std::vector<AtlasEntry> atlasTable;

    AtlasEntry LookupChar(const char ch);

    void DrawQuad(const float drawX, const float drawY, const float drawX2, const float drawY2,
		  const float srcX, const float srcY, const float srcX2, const float srcY2);

	/*
	Copying is disallowed.
	*/
	Font(Font&);
	Font& operator=(const Font&);

public:

    /*
      Load a texture atlas created by this program
     */
    Font(
	const std::string& atlasPath,
	const std::string& amfPath,
	const unsigned int windowWidth, const unsigned int windowHeight,
	const float fontScale
	);
    ~Font();

    void DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str);

    void DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str, const Vector3f& color);
};
