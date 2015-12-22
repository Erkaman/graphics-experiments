#pragma once

struct GLFWwindow;


enum GuiMode {
    ModifyTerrainMode,
    DrawTextureMode
};

class Gui {

private:

    int m_guiMode;

public:

    Gui(GLFWwindow* window);
    ~Gui();

    // call this function at the beginning of a new frame.
    void NewFrame(const float guiVerticalScale);

    void Render(int windowWidth, int windowHeight);

    int GetGuiMode()const;

};
