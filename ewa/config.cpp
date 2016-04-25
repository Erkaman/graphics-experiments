#include "config.hpp"

Config::Config() {

    m_zNear = 0.1f;
    m_zFar = 500.0f;

//    m_ambientLight = Vector3f(0.3);
    m_ambientLight = Vector3f(0.4);

    m_sceneLight = Vector3f(0.3);


    m_gui = false;
}

float Config::GetZNear() const {
    return m_zNear;
}

float Config::GetZFar() const {
    return m_zFar;
}


bool Config::IsGui() const {
    return m_gui;
}

void Config::SetGui(const bool gui) {
    m_gui = gui;
}


void Config::SetWorldFilename(const std::string& str) {
    m_worldFilename = str;
}

std::string Config::GetWorldFilename()const {
    return m_worldFilename;
}


Vector3f Config::GetAmbientLight()const {
    return m_ambientLight;
}

Vector3f Config::GetSceneLight()const {
    return m_sceneLight;
}
