#pragma once

#include <string>

class FPSManager  {

private:

    int m_maxFPS;
    const double m_targetFrameDuration;
    double m_frameStartTime;
    double m_frameEndTime;
    double m_lastReportTime;
    int m_frameCount;
    std::string fpsString;

public:

    FPSManager(const int maxFPS=30);

    // should be called at the start of every frame.
    double ManageFPS();
    void Start();

    std::string GetFpsString();

};
