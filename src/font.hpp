#pragma once

#include <memory>
#include <string>
#include <vector>

#include "math/vector2f.hpp"

class Texture;
class VBO;
class ShaderProgram;
class Color;

class Font {

private:

    std::unique_ptr<Texture> m_fontTexture;

    std::unique_ptr<VBO> m_vertexBuffer;

    const unsigned int m_fontTextureWidth;
    const unsigned int m_fontTextureHeight;

    const unsigned int m_fontCellWidth;
    const unsigned int m_fontCellHeight;


    // used to position the text within the window.
    const float m_scaleX;
    const float m_scaleY;

    std::vector<Vector2f> fontTable;

    Vector2f LookupChar(const char ch);

    void DrawQuad(const float drawX, const float drawY, const float drawX2, const float drawY2,
		  const float srcX, const float srcY, const float srcX2, const float srcY2);

public:

    Font(
	const std::string& fontTexturePath,
	const unsigned int fontTextureWidth, const unsigned int fontTextureHeight,
	const unsigned int fontCellWidth, const unsigned int fontCellHeight,
	const unsigned int windowWidth, const unsigned int windowHeight);

    void DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str, const Color& color);
    void DrawString(ShaderProgram& fontShader, const float x, const float y, const std::string& str);
};
