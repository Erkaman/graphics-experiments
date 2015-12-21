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

    GLFWwindow* m_window;


public:

    void Update();

    bool WasPressed(int button);
    bool IsPressed(int button);

    static MouseState& GetInstance(){
	static MouseState instance;

	return instance;
    }

    void Init(GLFWwindow* window);


    float GetDeltaX() const {
	return m_deltaX;
    }

    float GetDeltaY() const {
	return m_deltaY;
    }

    float GetX() const;
    float GetY() const;
};
