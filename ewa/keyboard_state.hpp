#pragma once


struct GLFWwindow;

class KeyboardState {

private:

    GLFWwindow* m_window;

    bool* m_prevFrameKeyState;
    bool* m_curFrameKeyState;

    KeyboardState& operator=(const KeyboardState& other);
    KeyboardState(const KeyboardState& other);
    KeyboardState() {}

public:

    static KeyboardState& GetInstance();

    void Init(GLFWwindow* window);

    bool IsPressed(int key)const;

    bool WasPressed(int key);

    void Update();

};
