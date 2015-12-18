#include "keyboard_state.hpp"

#include "gl/gl_common.hpp"


#include "log.hpp"

bool KeyboardState::IsPressed(int key) const{
    return m_curFrameKeyState[key] == GLFW_PRESS;
}

bool KeyboardState::WasPressed(int key) {
    return
	m_prevFrameKeyState[key] == GLFW_PRESS &&
	m_curFrameKeyState[key] == GLFW_RELEASE;
}


KeyboardState& KeyboardState::GetInstance() {
    static KeyboardState instance;
    return instance;
}

void KeyboardState::Init(GLFWwindow* window) {
    m_window = window;

    m_prevFrameKeyState = new bool[GLFW_KEY_LAST];
    m_curFrameKeyState = new bool[GLFW_KEY_LAST];

    for(int i =0 ; i < GLFW_KEY_LAST; ++i) {
	m_prevFrameKeyState[i] = GLFW_RELEASE;
	m_curFrameKeyState[i] = GLFW_RELEASE;
    }
}

void KeyboardState::Update() {
    for(int i =0 ; i < GLFW_KEY_LAST; ++i) {
	m_prevFrameKeyState[i] = m_curFrameKeyState[i];
	m_curFrameKeyState[i] = glfwGetKey( m_window , i );
    }
}
