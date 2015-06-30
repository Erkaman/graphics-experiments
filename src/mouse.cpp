#include "mouse.hpp"

#include "gl/gl_common.hpp"
#include "log.hpp"

void Mouse::Update(GLFWwindow* window) {

    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);


    m_deltaX = (float)xpos - m_previousX;
    m_deltaY = (float)ypos - m_previousY;

//    LOG_I("delta: %f, %f", m_deltaX, m_deltaY);

    m_previousX = (float)xpos;
    m_previousY = (float)ypos;

}
