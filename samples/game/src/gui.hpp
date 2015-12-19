#pragma once

struct GLFWwindow;

class Gui {

public:

    Gui(GLFWwindow* window);
    ~Gui();

    // call this function at the beginning of a new frame.
    void NewFrame(const float guiVerticalScale);

    void Render(int windowWidth, int windowHeight);

};
