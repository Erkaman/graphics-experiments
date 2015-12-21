#include "mouse_state.hpp"

#include "gl/gl_common.hpp"
#include "log.hpp"


MouseState::MouseState():m_previousX(0), m_previousY(0) {

    m_prevFrameButtonState = new bool[GLFW_MOUSE_BUTTON_LAST];
    m_curFrameButtonState = new bool[GLFW_MOUSE_BUTTON_LAST];

    for(int i =0 ; i < GLFW_MOUSE_BUTTON_LAST; ++i) {

	m_prevFrameButtonState[i] = GLFW_RELEASE;
	m_curFrameButtonState[i] = GLFW_RELEASE;
    }
}


void MouseState::Update() {

    for(int i =0 ; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
	m_prevFrameButtonState[i] = m_curFrameButtonState[i];
	m_curFrameButtonState[i] = glfwGetMouseButton( m_window , i );
    }

    double xpos;
    double ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    m_deltaX = (float)xpos - m_previousX;
    m_deltaY = (float)ypos - m_previousY;

//    LOG_I("delta: %f, %f", m_deltaX, m_deltaY);

    m_previousX = (float)xpos;
    m_previousY = (float)ypos;



}

bool MouseState::WasPressed(int button) {
    return
	m_prevFrameButtonState[button] == GLFW_PRESS &&
	m_curFrameButtonState[button] == GLFW_RELEASE;
}


bool MouseState::IsPressed(int button) {
    return m_curFrameButtonState[button] == GLFW_PRESS;
}



float MouseState::GetX() const {
    double xpos;
    glfwGetCursorPos(m_window,&xpos,NULL);
    return xpos;
}

float MouseState::GetY() const {
    double ypos;
    glfwGetCursorPos(m_window,NULL, &ypos);
    return ypos;
}

void MouseState::Init(GLFWwindow* window) {
    m_window = window;
}
