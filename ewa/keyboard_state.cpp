#include "keyboard_state.hpp"


bool KeyboardState::IsPressed(int key) const{
    return glfwGetKey( m_window , key ) == GLFW_PRESS;
}

bool KeyboardState::WasPressed(int key) {

    if(glfwGetKey( m_window , key ) == GLFW_PRESS) {
	pressedKeys[key] = true;
    } else {


	if(pressedKeys[key] == true) {
	    pressedKeys[key] = false;

	    return true;
	} else  {

	    return false;
	}
    }

    return false;

}


KeyboardState& KeyboardState::GetInstance() {
    static KeyboardState instance;
    return instance;
}

void KeyboardState::Init(GLFWwindow* window) {
    m_window = window;

    for(int i =0 ; i < GLFW_KEY_LAST; ++i) {
	pressedKeys[i] = false;
    }
}
