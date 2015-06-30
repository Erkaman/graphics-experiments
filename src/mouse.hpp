#pragma once

struct GLFWwindow;

// a singelton class.
class Mouse {

private:
    Mouse():m_previousX(0), m_previousY(0) {};

    Mouse(Mouse const&);
    void operator=(Mouse const&);

    float m_previousX;
    float m_previousY;

    float m_deltaX;
    float m_deltaY;

public:

    void Update(GLFWwindow* window);

    static Mouse& getInstance(){
	static Mouse instance;

	return instance;
    }

    float GetDeltaX() const {
	return m_deltaX;
    }

    float GetDeltaY() const {
	return m_deltaY;
    }

};
