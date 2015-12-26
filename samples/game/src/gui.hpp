#pragma once

#include "ewa/math/vector3f.hpp"

struct GLFWwindow;

class GuiListener;

class Gui {

private:

    int m_guiMode;
    int m_drawTextureType;
    int m_inputMode;
    int m_axisMode;

    Vector3f m_translation;
    Vector3f m_rotation; // describes the rotation in euler angles.

    std::vector<GuiListener*> m_listeners;

    void ResetModelMode();

public:

    Gui(GLFWwindow* window);
    ~Gui();

    // call this function at the beginning of a new frame.
    void NewFrame(const float guiVerticalScale);

    void Render(int windowWidth, int windowHeight);
    void Update();

    int GetGuiMode()const;
    int GetDrawTextureType()const;

    Vector3f GetTranslation()const;
    Vector3f GetRotation()const;

    void AddListener(GuiListener* listener);

};
