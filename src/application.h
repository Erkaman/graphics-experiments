#pragma once

#include "precomp.h"

class Application {

private:

    void SetupOpenGL();
    void DoMainLoop();
    void Cleanup();

    GLFWwindow* window;

public:

    Application() {}

    void Start();

    virtual void Init() = 0;

};
