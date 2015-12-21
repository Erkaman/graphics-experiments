#include "gui_mouse_state.hpp"

#include "ewa/gl/gl_common.hpp"
#include "ewa/log.hpp"
#include "ewa/mouse_state.hpp"


float GuiMouseState::m_guiVerticalScale;
float GuiMouseState::m_x;
float GuiMouseState::m_y;
bool GuiMouseState::m_withinWindow;


void GuiMouseState::Update(int framebufferWidth, int framebufferHeight) {


    MouseState& ms = MouseState::GetInstance();

    m_x = ms.GetX()*2;
    m_y = ms.GetY()*2;

    if(framebufferWidth*m_guiVerticalScale > m_x) { // if click within gui ignore.
//	LOG_I("not in window");
	m_withinWindow = false;
    } else {
//	LOG_I("in window");

	m_withinWindow = true;
    }

}

float GuiMouseState::GetX() {
    return m_x;
}

float GuiMouseState::GetY() {
    return m_y;
}


bool GuiMouseState::isWithinWindow() {
    return m_withinWindow;
}


void GuiMouseState::Init(const float guiVerticalScale) {
    GuiMouseState::m_guiVerticalScale = guiVerticalScale;
}
