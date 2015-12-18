#pragma once

#include <string>
#include "gl/vao.hpp"

#pragma warning( disable : 4099 )


class GLFWwindow;

class ShaderProgram;

class Font;


class Application {

protected:

    GLFWwindow* m_window;


private:

    void SetupOpenGL();
    void DoMainLoop();
    void Cleanup();

    void Update_internal(const float delta);
    void RenderText_internal(const std::string& fpsString);

    bool m_running;

    VAO* m_vao;

    int m_width;
    int m_height;


public:


    void Start();

    int GetWindowWidth();
    int GetWindowHeight();


protected:

    Application(int argc, char *argv[], int width=800, int height=600);

    Font* m_font;
    ShaderProgram* m_fontShader;

    virtual void Render() = 0;
    virtual void Init() = 0;
    virtual void Update(const float delta) = 0;
    virtual void RenderText() {}


    void SetViewport();

    void SetWindowTitle(const std::string& title);

    int GetFramebufferWidth();
    int GetFramebufferHeight();


};
