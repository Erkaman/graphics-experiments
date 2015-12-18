#include "gui.hpp"

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "ewa/log.hpp"

Gui::Gui(GLFWwindow* window) {

    // init gui:
    if(ImGui_ImplGlfwGL3_Init(window, true)) {
	LOG_I("IMGUI initialization succeeded");
    } else {
	LOG_E("IMGUI initialization failed");
    }
}


Gui::~Gui() {
    ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
    ImGui::Shutdown();
}


void Gui::NewFrame(const float guiVerticalScale) {
    ImGui_ImplGlfwGL3_NewFrame(guiVerticalScale);
}

void Gui::Render(int windowWidth, int windowHeight) {
    static float f = 0.0f;

    ImGuiWindowFlags window_flags = 0;
//    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_ShowBorders;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    //window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;

    float bg_alpha = 1.0f; // <0: default

    bool p_opened = true;

    ImGui::Begin("ImGui Demo", &p_opened,
		 ImVec2(windowWidth,windowHeight), bg_alpha, window_flags);

    ImGui::SetWindowPos(ImVec2(0,0) );

    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    ImGui::End();

    ImGui::Render();

}
