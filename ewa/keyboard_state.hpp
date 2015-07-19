#pragma once

class GLFWwindow;

class KeyboardState {

private:

    GLFWwindow* m_window;

    KeyboardState& operator=(const KeyboardState& other);
    KeyboardState(const KeyboardState& other);
    KeyboardState() {}

public:

    static KeyboardState& GetInstance();

    void Init(GLFWwindow* window);

    bool IsPressed(int key)const;

};
