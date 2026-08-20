#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"
#include <string>

enum class InGameMenuAction {
    None,
    Resume,
    RestartLevel,
    MainMenu
};

class InGameMenu {
public:
    InGameMenu(Resources& res);
    ~InGameMenu() = default;

    void open();
    void close();
    void toggle();
    bool isOpen() const { return m_isOpen; }

    InGameMenuAction update(float dt);
    void draw();

    // Top-right Menu button helper
    static Rectangle GetMenuButtonRect() { return { 680.0f, 0.0f, 110.0f, 36.0f }; }
    void drawMenuButton(bool hovered, bool pressed);

private:
    void draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed);

    Resources& m_res;
    BitmapFont m_font;
    bool m_isOpen = false;

    // Menu UI textures
    Texture2D m_menuBack;
    Texture2D m_backToGameBtn;
    Texture2D m_backToGameBtnHl;

    // Slider textures and state
    Texture2D m_sliderSlot;
    Texture2D m_sliderKnob;
    bool m_isDraggingMusicSlider = false;
    bool m_isDraggingSfxSlider = false;

    // 3-slice button textures
    Texture2D m_btnLeft;
    Texture2D m_btnMiddle;
    Texture2D m_btnRight;
    Texture2D m_btnLeftDown;
    Texture2D m_btnMiddleDown;
    Texture2D m_btnRightDown;
};
