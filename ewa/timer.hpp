#pragma once

#include <chrono>

class Timer {

private:

    std::chrono::time_point<std::chrono::steady_clock> m_startTime;

public:

    Timer() {}

    // start timer.
    void Start();

    // stop timer, and return measured time in milliseconds
    double Stop();

};
