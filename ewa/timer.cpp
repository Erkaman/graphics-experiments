#include "timer.hpp"

void Timer::Start() {
    m_startTime = std::chrono::steady_clock::now();
}


double Timer::Stop() {
    auto endTime = std::chrono::steady_clock::now();

    auto diff = endTime - m_startTime;

    return std::chrono::duration <double, std::milli> (diff).count();
}
