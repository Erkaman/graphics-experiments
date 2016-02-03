#pragma once

#include <imgui.h>

class GLFWwindow;

bool    ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks);

bool ImGui_ImplGlfwGL3_CreateDeviceObjects();


void ImGui_ImplGlfwGL3_InvalidateDeviceObjects();

void ImGui_ImplGlfwGL3_NewFrame(float guiVerticalScale);

GLFWwindow* ImGui_ImplGlfwGL3_GetWindow();

float GetDeltaScroll();

void SetDeltaScroll(float deltaScroll_);
