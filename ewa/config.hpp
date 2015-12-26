#pragma once

#include <string>

class Config {

private:

    Config(Config const&);
    void operator=(Config const&);

    Config();

    // near and far values for the camera.
    float m_zNear;
    float m_zFar;
    bool m_gui;
    std::string m_worldFilename;

public:

    static Config& GetInstance(){
	static Config instance;

	return instance;
    }

    float GetZNear() const;
    float GetZFar() const;

    bool IsGui() const;
    void SetGui(const bool gui);

    void SetWorldFilename(const std::string& str);
    std::string GetWorldFilename()const;

};
