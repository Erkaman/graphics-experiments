#pragma once

class Config {

private:

    Config(Config const&);
    void operator=(Config const&);

    Config();

    // near and far values for the camera.
    float m_zNear;
    float m_zFar;

public:

    static Config& GetInstance(){
	static Config instance;

	return instance;
    }

    float GetZNear() const;
    float GetZFar() const;



};
