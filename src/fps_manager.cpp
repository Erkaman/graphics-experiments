#include "fps_manager.hpp"

#include "gl/gl_common.hpp"
#include "log.hpp"
#include <chrono>
#include <thread>

std::chrono::milliseconds oneMilliSecond(1);


FPSManager::FPSManager(const int maxFPS): m_maxFPS(maxFPS), m_targetFrameDuration(1.0/m_maxFPS),m_frameStartTime(0),m_frameEndTime(0) {
}

double FPSManager::ManageFPS() {

    m_frameEndTime = glfwGetTime();

    double frameDuration = m_frameEndTime - m_frameStartTime;

    if ((m_frameEndTime - m_lastReportTime) > 1.0)
    {
	m_lastReportTime = m_frameEndTime;
	double currentFps =  (double)m_frameCount / 1.0;
	m_frameCount = 1;

	fpsString = "fps: " + std::to_string(currentFps);
    } else {
	++m_frameCount;
    }

    const double sleepDuration = m_targetFrameDuration - frameDuration;

    if (sleepDuration > 0.0) {
	std::this_thread::sleep_for(sleepDuration * oneMilliSecond);
    }

    m_frameStartTime = glfwGetTime();

    return frameDuration + (m_frameStartTime - m_frameEndTime);
}

void FPSManager::Start() {
    m_frameStartTime = glfwGetTime();
    m_lastReportTime = m_frameStartTime;
    m_frameCount = 0;
}

std::string FPSManager::GetFpsString() {
    return fpsString;
}
