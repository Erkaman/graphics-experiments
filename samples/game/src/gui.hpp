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
    int m_terrainMode;

    Vector3f m_translation;
    Vector3f m_rotation; // describes the rotation in euler angles.
    float m_scale;

    std::vector<GuiListener*> m_listeners;

    void ResetModelMode();

    int m_cursorSize;
    int m_strength;
    float m_noiseScale;
    int m_smoothRadius;
    int m_grassClusterSize;

    int m_aoSamples;
    int m_aoWaveLength;
    int m_aoAmplitude;
    float m_aoDistAttenuation;

    bool m_aoOnly;
    bool m_enableAo;

    void RadiusSlider();

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
    float GetScale()const;

    void AddListener(GuiListener* listener);

    int GetCursorSize()const;
    void SetCursorSize(int cursorSize);

    float GetStrength()const;


    int GetTerrainMode()const;

    float GetNoiseScale()const;
    int GetSmoothRadius()const;


    bool isAoOnly()const;
    bool IsEnableAo()const;

    int GetGrassClusterSize()const;

};
