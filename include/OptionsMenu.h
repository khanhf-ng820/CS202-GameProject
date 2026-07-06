#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"

struct ResolutionPreset {
    int width;
    int height;
    const char* label;
};

static const ResolutionPreset RESOLUTION_PRESETS[] = {
    { 800, 480, "800 x 480 (Test)" },
    { 1280, 768, "1280 x 768 (Default)" },
    { 1600, 960, "1600 x 960 (Large)" },
    { 1920, 1152, "1920 x 1152 (Full HD)" }
};
static const int NUM_RESOLUTIONS = 4;

class OptionsMenu {
public:
    OptionsMenu(Resources& res);
    ~OptionsMenu() = default;

    void update(float dt, bool& showOptions, int& currentWidth, int& currentHeight);
    void draw();

private:
    void draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed);

    Resources& m_res;
    BitmapFont m_font;

    int m_selectedResolutionIndex;
    int m_activeResolutionIndex;

    // Option menu UI textures
    Texture2D m_menuBack;
    Texture2D m_checkboxUnchecked;
    Texture2D m_checkboxChecked;
    Texture2D m_backToGameBtn;
    Texture2D m_backToGameBtnHl;

    // 3-slice button textures
    Texture2D m_btnLeft;
    Texture2D m_btnMiddle;
    Texture2D m_btnRight;
    Texture2D m_btnLeftDown;
    Texture2D m_btnMiddleDown;
    Texture2D m_btnRightDown;
};
