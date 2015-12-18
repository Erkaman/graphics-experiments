#pragma once

struct GLFWwindow;

// a singelton class.
class MouseState {

private:
    MouseState();

    MouseState(MouseState const&);
    void operator=(MouseState const&);

    float m_previousX;
    float m_previousY;

    float m_deltaX;
    float m_deltaY;

    bool* m_prevFrameButtonState;
    bool* m_curFrameButtonState;


public:

    void Update(GLFWwindow* window);

    bool WasPressed(int button);

    static MouseState& GetInstance(){
	static MouseState instance;

	return instance;
    }

    float GetDeltaX() const {
	return m_deltaX;
    }

    float GetDeltaY() const {
	return m_deltaY;
    }



};
