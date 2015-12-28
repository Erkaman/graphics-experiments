#pragma once

#include <string>

enum GuiMode {
    ModifyTerrainMode,
    DrawTextureMode,
    ModelMode
};

enum TerrainMode {
    ModifyElevationMode,
    DistortMode
};



// if in DrawTextureMode, specify which texture we are currently drawing.
enum DrawTextureType {
    GrassTexture = 0,
    DirtTexture = 1,
    RockTexture = 2,
    // add new textures HERE!

    EraserTexture = 3
};

enum InputMode {
    InputTranslateMode,
    InputRotateMode,
    InputNoneMode,
};

// which axis you are inputting for, when in either InputTranslateMode or InputRotateMode.
enum AxisMode {
    XAxis,
    YAxis,
    ZAxis,
    NoneAxis
};

inline std::string AxisModeToStr(int axisMode) {

    if(axisMode == XAxis) {
	return "XAxis: ";
    }else if(axisMode == YAxis) {
	return "YAxis: ";
    }else if(axisMode == ZAxis) {
	return "ZAxis: ";
    } else {
	return "NoneAxis";
    }
}
