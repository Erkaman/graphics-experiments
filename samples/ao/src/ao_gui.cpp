#include "ao_gui.hpp"
#include "ao_application.hpp"

#include"ewa/imgui_wrapper.hpp"

#include "ewa/log.hpp"

using std::string;

AoGui::AoGui(GLFWwindow* window, AoApplication* app): m_app(app) {

    // init gui:
    if(ImGui_ImplGlfwGL3_Init(window, true)) {
	LOG_I("IMGUI initialization succeeded");
    } else {
	LOG_E("IMGUI initialization failed");
    }

    m_samples = 256;
}


AoGui::~AoGui() {
    ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
    ImGui::Shutdown();
}


void AoGui::NewFrame(const float guiVerticalScale) {
    ImGui_ImplGlfwGL3_NewFrame(guiVerticalScale);
}

void AoGui::Render(int windowWidth, int windowHeight) {
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

    ImGui::Text("Global mode:");

    if (ImGui::Button("Bake AO")) {
	m_app->BakeAo(m_samples);
    }

    ImGui::SliderInt("Samples", &m_samples, 1, 1000);

    ImGui::End();

    ImGui::Render();
}

void AoGui::Update() {

}
