#pragma once

#include "gl/gl_common.hpp"

struct GLFWwindow;

class KeyboardState {

private:

    GLFWwindow* m_window;

    bool pressedKeys[GLFW_KEY_LAST];

    KeyboardState& operator=(const KeyboardState& other);
    KeyboardState(const KeyboardState& other);
    KeyboardState() {}

public:

    static KeyboardState& GetInstance();

    void Init(GLFWwindow* window);

    bool IsPressed(int key)const;

    bool WasPressed(int key);

};
