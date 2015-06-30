#pragma once

#include <string>

class FPSManager  {

private:

    const int m_maxFPS;
    const float m_targetFrameDuration;
    float m_frameStartTime;
    float m_frameEndTime;
    float m_lastReportTime;
    int m_frameCount;
    std::string fpsString;

public:

    FPSManager(const int maxFPS=30);

	FPSManager(const FPSManager&) = delete;
	FPSManager& operator=(const FPSManager&) = delete;

    // should be called at the start of every frame.
    float ManageFPS();
    void Start();

    std::string GetFpsString();

};
