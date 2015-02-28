#pragma once

#include "precomp.hpp"

class Application {

private:

    void SetupOpenGL();
    void DoMainLoop();
    void Cleanup();

    void Update_internal();

    GLFWwindow* window;
    bool running;

public:

    Application() {}
    void Start();


protected:
    virtual void Render() = 0;
    virtual void Init() = 0;
    virtual void Update() = 0;

    void SetViewport();

};
