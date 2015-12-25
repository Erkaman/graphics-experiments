#pragma once

enum GuiMode {
    ModifyTerrainMode,
    DrawTextureMode,
    ModelMode

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
    InputNoneMode,
};

// which axis you are inputting for, when in either InputTranslateMode or InputRotateMode.
enum AxisMode {
    XAxis,
    YAxis,
    ZAxis,
    NoneAxis
};
