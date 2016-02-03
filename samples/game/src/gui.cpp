#include "gui.hpp"
#include "gui_enum.hpp"
#include "gui_listener.hpp"

#include "ewa/log.hpp"
#include "ewa/file.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/mouse_state.hpp"

#include "nfd.h"

#include"imgui_wrapper.hpp"

#if !defined(_WIN32)
#include "cocoa_util.hpp"
#endif

#include <stdio.h>




constexpr int MIN_CURSOR_SIZE = 3;
constexpr int MAX_CURSOR_SIZE = 90;
constexpr int DEFAULT_RADIUS = 35;

constexpr float MIN_NOISE_SCALE = 0.0004;
constexpr float MAX_NOISE_SCALE = 0.1;
constexpr float DEFAULT_NOISE_SCALE = 0.04;

constexpr int MIN_SMOOTH_RADIUS = 1;
constexpr int MAX_SMOOTH_RADIUS = 4;
constexpr int DEFAULT_SMOOTH_RADIUS = 1;


constexpr int MIN_AO_SAMPLES = 4;
constexpr int MAX_AO_SAMPLES = 70;
constexpr int DEFAULT_AO_SAMPLES = 32;


constexpr float MIN_AO_DIST_ATTENUATION = 0.1;
constexpr float MAX_AO_DIST_ATTENUATION = 5;
constexpr float DEFAULT_AO_DIST_ATTENUATION = 1.7;

constexpr int MIN_AO_WAVE_LENGTH = 50;
constexpr int MAX_AO_WAVE_LENGTH = 600;
constexpr int DEFAULT_AO_WAVE_LENGTH = 300;






using std::string;

Gui::Gui(GLFWwindow* window) {

    m_guiMode = ModifyTerrainMode;
    m_drawTextureType = GrassTexture;
    m_inputMode = InputNoneMode;
    m_axisMode = NoneAxis;
    m_translation = Vector3f(0);
    m_rotation = Vector3f(0);
    m_scale = 1.0f;

    m_cursorSize = DEFAULT_RADIUS;
    m_strength = 10;
    m_noiseScale = DEFAULT_NOISE_SCALE;
    m_smoothRadius = DEFAULT_SMOOTH_RADIUS;

    m_aoSamples = DEFAULT_AO_SAMPLES;
    m_aoWaveLength = DEFAULT_AO_WAVE_LENGTH;
    m_aoAmplitude = 200;
    m_aoDistAttenuation = DEFAULT_AO_DIST_ATTENUATION;

    m_terrainMode = ModifyElevationMode;

    m_aoOnly = false;

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

void Gui::RadiusSlider() {
	int oldCursorSize = m_cursorSize;
	ImGui::SliderInt("Radius", &m_cursorSize, MIN_CURSOR_SIZE, MAX_CURSOR_SIZE);
	if(oldCursorSize != m_cursorSize) {
	    SetCursorSize(m_cursorSize);
	}
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

    ImGui::Text("Global mode:");

    ImGui::RadioButton("MT", &m_guiMode, ModifyTerrainMode); ImGui::SameLine();
    ImGui::RadioButton("DT", &m_guiMode, DrawTextureMode);  ImGui::SameLine();
    ImGui::RadioButton("M", &m_guiMode, ModelMode);

    if(m_guiMode == ModifyTerrainMode) {

	ImGui::Text("Local mode:");

	ImGui::RadioButton("Elevation", &m_terrainMode, ModifyElevationMode);
	ImGui::RadioButton("Distort", &m_terrainMode, DistortMode);
	ImGui::RadioButton("Smooth", &m_terrainMode, SmoothMode);
	ImGui::RadioButton("Level", &m_terrainMode, LevelMode);

	RadiusSlider();


	ImGui::SliderInt("Strength", &m_strength, 1, 35);

	ImGui::SliderFloat("Noise Scale", &m_noiseScale, MIN_NOISE_SCALE, MAX_NOISE_SCALE);
	ImGui::SliderInt("Smooth Radius", &m_smoothRadius, MIN_SMOOTH_RADIUS, MAX_SMOOTH_RADIUS);
	ImGui::Checkbox("AO Only", &m_aoOnly);






	if (ImGui::Button("Bake AO")) {

	    for(GuiListener* listener : m_listeners) {

		listener->BakeAo(
		    m_aoSamples,
		    m_aoWaveLength,
		    m_aoAmplitude,
		    m_aoDistAttenuation

		    );
	    }
	}
	ImGui::SliderInt("AO samples", &m_aoSamples, MIN_AO_SAMPLES, MAX_AO_SAMPLES);
	ImGui::SliderFloat("AO dist attenuation", &m_aoDistAttenuation, MIN_AO_DIST_ATTENUATION, MAX_AO_DIST_ATTENUATION );


	ImGui::SliderInt("AO wave length", &m_aoWaveLength, MIN_AO_WAVE_LENGTH, MAX_AO_WAVE_LENGTH );



    } else if(m_guiMode == DrawTextureMode) {

	RadiusSlider();


        ImGui::Combo("Texture", &m_drawTextureType, "Grass\0Dirt\0Rock\0Eraser\0\0");   // Combo using values packed in a single
    } else {

	ImGui::Text("lol:");

	string xs = "x: ";
	xs += std::to_string(m_translation.x);
	ImGui::Text(xs.c_str() );

	string mode = "state: ";
	if(m_inputMode == InputNoneMode) {
	    mode += "none";
	}  else if(m_inputMode == InputTranslateMode) {
	    mode += "translate: ";

	    mode += AxisModeToStr(m_axisMode);
	}  else if(m_inputMode == InputRotateMode) {
	    mode += "rotate: ";

	    mode += AxisModeToStr(m_axisMode);
	}  else if(m_inputMode == InputScaleMode) {
	    mode += "scale: ";
	}

	ImGui::Text(mode.c_str() );

#if defined(__APPLE__)
	if (ImGui::Button("Add Model")) {

	    nfdchar_t *outPath = NULL;

	    nfdresult_t result = NFD_OpenDialog( "eob", "obj/", &outPath );

	    FocusWindow(ImGui_ImplGlfwGL3_GetWindow() );


	    if ( result == NFD_OKAY ) {

		string path(outPath);

		size_t lastSlashIndex = path.rfind("/");

		size_t secondToLastSlashIndex = path.rfind("/", lastSlashIndex-1);

		string objDir = path.substr(secondToLastSlashIndex+1, lastSlashIndex-secondToLastSlashIndex-1);

		// we only allow files to be loaded from the directory obj/
		if(objDir == "obj" ) {

		    string objPath =
			path.substr(secondToLastSlashIndex+1, string::npos);

		    for(GuiListener* listener : m_listeners) {
			listener->ModelAdded(objPath);
		    }

		} else {
 		    LOG_W("You can only load obj files from obj/");
		}

//		LOG_I("found path: %s",
//		      .c_str() );


	    }
	}
#else

	LOG_W("This feature is not yet supported on windows");
#endif

	if (ImGui::Button("Duplicate")) {

	    for(GuiListener* listener : m_listeners) {
		listener->Duplicate();
	    }
	}

	if (ImGui::Button("Delete")) {

	    for(GuiListener* listener : m_listeners) {
		listener->Delete();
	    }
	}


    }



    ImGui::End();

    ImGui::Render();

}
//             ImGui::LogButtons();


int Gui::GetGuiMode()const {
    return m_guiMode;
}


int Gui::GetDrawTextureType()const {
    return m_drawTextureType;
}


void Gui::ResetModelMode() {

    m_translation = Vector3f(0);
    m_rotation = Vector3f(0);
    m_scale =1.0f;

    m_inputMode = InputNoneMode;
    m_axisMode = NoneAxis;
}

void Gui::Update() {

    KeyboardState& kbs = KeyboardState::GetInstance();
    MouseState& ms = MouseState::GetInstance();


    if(m_guiMode == ModelMode) {

	if(m_inputMode == InputNoneMode) {

	    if( kbs.WasPressed(GLFW_KEY_G) ) {
		m_inputMode = InputTranslateMode;
	    } else if( kbs.WasPressed(GLFW_KEY_R) ) {
		m_inputMode = InputRotateMode;
	    }else if( kbs.WasPressed(GLFW_KEY_E) ) {
		m_inputMode = InputScaleMode;
	    }

	}else if(m_inputMode == InputRotateMode) {

	    if(kbs.IsPressed(GLFW_KEY_BACKSPACE) ) {
		ResetModelMode();

	    } else if(kbs.IsPressed(GLFW_KEY_ENTER) ) {

		for(GuiListener* listener : m_listeners) {
		    listener->RotationAccepted();
		}

		ResetModelMode();

	    } else {

		if(m_axisMode == NoneAxis) {

		    // at this point, you can input which axis you wish to translate for.

		    if( kbs.WasPressed(GLFW_KEY_X) ) {
			m_axisMode = XAxis;
		    } else if( kbs.WasPressed(GLFW_KEY_Y) ) {
			m_axisMode = YAxis;
		    } else if( kbs.WasPressed(GLFW_KEY_Z) ) {
			m_axisMode = ZAxis;
		    }

		} else {
		    float* tr = (float*)&m_rotation;

		    tr[m_axisMode] += ms.GetDeltaX() * 0.01;
		}
	    }

	}else if(m_inputMode == InputTranslateMode) {

	    if(kbs.IsPressed(GLFW_KEY_BACKSPACE) ) {
		ResetModelMode();

	    } else if(kbs.IsPressed(GLFW_KEY_ENTER) ) {

		for(GuiListener* listener : m_listeners) {
		    listener->TranslationAccepted();
		}

		ResetModelMode();

	    } else {

		if(m_axisMode == NoneAxis) {

		    // at this point, you can input which axis you wish to translate for.

		    if( kbs.WasPressed(GLFW_KEY_X) ) {
			m_axisMode = XAxis;
		    } else if( kbs.WasPressed(GLFW_KEY_Y) ) {
			m_axisMode = YAxis;
		    } else if( kbs.WasPressed(GLFW_KEY_Z) ) {
			m_axisMode = ZAxis;
		    }

		} else {
		    // get keyboard number input.

		    float* tr = (float*)&m_translation;

		    tr[m_axisMode] += ms.GetDeltaX() * 0.1;
		}
	    }
	}else if(m_inputMode == InputScaleMode) {

	    if(kbs.IsPressed(GLFW_KEY_BACKSPACE) ) {
		ResetModelMode();

	    } else if(kbs.IsPressed(GLFW_KEY_ENTER) ) {

		for(GuiListener* listener : m_listeners) {
		    listener->ScaleAccepted();
		}

		ResetModelMode();

	    } else {

		m_scale += ms.GetDeltaX() * 0.01;
	    }
	}





    } else if(m_guiMode == ModifyTerrainMode || m_guiMode == DrawTextureMode) {

	float diff = GetDeltaScroll();

	SetCursorSize( GetCursorSize() - 1.5 * diff  );

	SetDeltaScroll(0);

    }

}

Vector3f Gui::GetTranslation()const {
    return m_translation;
}

Vector3f Gui::GetRotation()const {
    return m_rotation;
}

float Gui::GetScale()const {
    return m_scale;
}


void Gui::AddListener(GuiListener* listener) {
    m_listeners.push_back(listener);
}


int Gui::GetCursorSize()const {
    return m_cursorSize;
}


void Gui::SetCursorSize(int cursorSize) {

    if(cursorSize < MIN_CURSOR_SIZE) {
	cursorSize = MIN_CURSOR_SIZE;
    }

    if(cursorSize > MAX_CURSOR_SIZE) {
	cursorSize = MAX_CURSOR_SIZE;
    }

    m_cursorSize = cursorSize;

    for(GuiListener* listener : m_listeners) {
	listener->CursorSizeChanged();
    }

}

float Gui::GetStrength()const {
    return 1.0f / (40.0 - m_strength);
}

int Gui::GetTerrainMode()const {
    return m_terrainMode;
}


float Gui::GetNoiseScale()const {
    return m_noiseScale;
}

int Gui::GetSmoothRadius()const {
    return m_smoothRadius;
}



bool Gui::isAoOnly()const {
    return m_aoOnly;
}
