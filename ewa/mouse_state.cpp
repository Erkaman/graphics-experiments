#include "mouse_state.hpp"

#include "gl/gl_common.hpp"
#include "log.hpp"


MouseState::MouseState():m_previousX(0), m_previousY(0) {

    pressedButtons = new bool[GLFW_MOUSE_BUTTON_LAST];

    for(int i =0 ; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
	pressedButtons[i] = false;
    }
}


void MouseState::Update(GLFWwindow* window) {

    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    m_deltaX = (float)xpos - m_previousX;
    m_deltaY = (float)ypos - m_previousY;

//    LOG_I("delta: %f, %f", m_deltaX, m_deltaY);

    m_previousX = (float)xpos;
    m_previousY = (float)ypos;

}


bool MouseState::WasPressed(int button) {

}
