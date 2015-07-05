#pragma once

#include <string>
#include <memory>
#include "gl/vao.hpp"

#pragma warning( disable : 4099 )


class GLFWwindow;

class ShaderProgram;


class Font;


class Application {

private:

    void SetupOpenGL();
    void DoMainLoop();
    void Cleanup();

    void Update_internal(const float delta);
    void RenderText_internal(const std::string& fpsString);

    GLFWwindow* window;
    bool running;

    VAO* vao;

public:


    void Start();

    int GetWindowWidth();
    int GetWindowHeight();


protected:

    std::unique_ptr<Font> m_font;
    std::unique_ptr<ShaderProgram> m_fontShader;

    virtual void Render() = 0;
    virtual void Init() = 0;
    virtual void Update(const float delta) = 0;
    virtual void RenderText() {}


    void SetViewport();

    int GetKey(int key);

    void SetWindowTitle(const std::string& title);


};
