#pragma once

#include "ewa/math/vector3f.hpp"

struct GLFWwindow;


class Gui {

private:

    int m_guiMode;
    int m_drawTextureType;
    int m_inputMode;
    int m_axisMode;

    Vector3f m_translate;
    Vector3f m_rotate; // describes the rotation in euler angles.

    bool m_accepted;

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

    Vector3f GetTranslate()const;
    Vector3f GetRotate()const;

    bool WasAccepted()const;


};
