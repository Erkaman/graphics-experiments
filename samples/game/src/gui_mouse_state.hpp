#pragma once

struct GLFWwindow;

class GuiMouseState {

private:

    static float m_x;
    static float m_y;
    static bool m_withinWindow; // is inside the window and not outside the gui?
    static float m_guiVerticalScale;

public:

    static void Update(int framebufferWidth, int framebufferHeight);

    static float GetX();
    static float GetY();
    static bool isWithinWindow();

    static void Init(const float guiVerticalScale);
};
