#pragma once

#include "ewa/math/vector3f.hpp"

struct GLFWwindow;
class GuiListener;
class AoApplication;

class AoGui {

private:

    int m_guiMode;
    int m_drawTextureType;
    int m_inputMode;
    int m_axisMode;
    int m_terrainMode;
    int m_samples;

    Vector3f m_translation;
    Vector3f m_rotation; // describes the rotation in euler angles.
    float m_scale;

    void ResetModelMode();

    AoApplication* m_app;


public:

    AoGui(GLFWwindow* window, AoApplication* app);
    ~AoGui();

    // call this function at the beginning of a new frame.
    void NewFrame(const float guiVerticalScale);

    void Render(int windowWidth, int windowHeight);
    void Update();

};
