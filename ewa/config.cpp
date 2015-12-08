#include "config.hpp"

Config::Config() {

    m_zNear = 0.1f;
    m_zFar = 300.0f;
}

float Config::GetZNear() const {
    return m_zNear;
}

float Config::GetZFar() const {
    return m_zFar;
}
