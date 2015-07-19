#include "keyboard_state.hpp"

#include "gl/gl_common.hpp"

bool KeyboardState::IsPressed(int key) const{
    return glfwGetKey( m_window , key ) == GLFW_PRESS;
}

KeyboardState& KeyboardState::GetInstance() {
    static KeyboardState instance;
    return instance;
}

void KeyboardState::Init(GLFWwindow* window) {
    m_window = window;
}
