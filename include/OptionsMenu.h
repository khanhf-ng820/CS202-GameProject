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
    { 1200, 900, "1200 x 900 (Default)" },
    { 800, 600, "800 x 600 (Compact)" },
    { 1024, 768, "1024 x 768 (Standard)" },
    { 1280, 960, "1280 x 960 (Large)" },
    { 1600, 1200, "1600 x 1200 (Full HD)" }
};
static const int NUM_RESOLUTIONS = 5;

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
