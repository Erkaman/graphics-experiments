#pragma once

class GLFWwindow;

// a singelton class.
class Mouse {

private:
    Mouse():m_previousX(0), m_previousY(0) {};

    Mouse(Mouse const&);
    void operator=(Mouse const&);

    double m_previousX;
    double m_previousY;

    double m_deltaX;
    double m_deltaY;

public:

    void Update(GLFWwindow* window);

    static Mouse& getInstance(){
	static Mouse instance;

	return instance;
    }

    double GetDeltaX() const {
	return m_deltaX;
    }

    double GetDeltaY() const {
	return m_deltaY;
    }

};
