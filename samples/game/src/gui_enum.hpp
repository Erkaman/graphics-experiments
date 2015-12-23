#pragma once

enum GuiMode {
    ModifyTerrainMode,
    DrawTextureMode
};

// if in DrawTextureMode, specify which texture we are currently drawing.
enum DrawTextureType {
    GrassTexture = 0,
    DirtTexture = 1,
    RockTexture = 2,
    // add new textures HERE!

    EraserTexture = 3


};
