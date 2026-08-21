#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "BitmapFont.h"
#include <string>
#include <vector>

enum class LevelSelectAction {
    None,
    PlayLevel,
    PlayLevel1,
    BackToMainMenu
};

enum class LevelStageMode {
    Day,
    Night
};

class LevelSelectMenu {
public:
    LevelSelectMenu(Resources& res);
    ~LevelSelectMenu() = default;

    void update(float dt, bool& showLevelSelect);
    void draw();

    LevelSelectAction getAction() const { return m_action; }
    int getSelectedLevel() const { return m_selectedLevel; }
    void resetAction() { m_action = LevelSelectAction::None; }

private:
    Resources& m_res;
    LevelSelectAction m_action = LevelSelectAction::None;
    int m_selectedLevel = 1;
    LevelStageMode m_stageMode = LevelStageMode::Day;

    // Background Sky & Wall/Lawn Foreground & Scenery
    Texture2D m_skyTex;
    Texture2D m_bgHorizon;
    Texture2D m_bgDistantTree;
    Texture2D m_bgBush;
    Texture2D m_bgTex;
    Texture2D m_bgTreeLeft;

    // Stage buttons textures
    Texture2D m_btnDay;
    Texture2D m_btnDaySel;
    Texture2D m_btnDayDis;

    Texture2D m_btnNight;
    Texture2D m_btnNightSel;
    Texture2D m_btnNightDis;

    Texture2D m_btnPoolDis;
    Texture2D m_btnFogDis;
    Texture2D m_btnRoofDis;

    // Upper Level Card frames
    Texture2D m_panelFrame;
    Texture2D m_specialFrame;
    Texture2D m_specialFrameRip;
    Texture2D m_cloudyFrame;
    Texture2D m_groundDay;
    Texture2D m_groundNight;

    // UI Buttons (Back / Play)
    Texture2D m_btnIndex;
    Texture2D m_btnIndexHl;

    // 3-slice buttons for Play button
    Texture2D m_btnLeft;
    Texture2D m_btnMiddle;
    Texture2D m_btnRight;
    Texture2D m_btnLeftDown;
    Texture2D m_btnMiddleDown;
    Texture2D m_btnRightDown;

    BitmapFont m_font;
    bool m_fontLoaded = false;

    BitmapFont m_brianneFont;
    bool m_brianneLoaded = false;

    Reanimation m_zombiePreview;

    // Helper functions
    bool isStageHovered(Vector2 mousePos, Rectangle screenRect, const std::string& texName, float scale);
    void drawTombstoneButton(const std::string& label, Rectangle screenRect, Texture2D normalTex, Texture2D selTex, bool hovered, bool selected, bool enabled, Vector2 nativeCenter, float angleDeg, Vector2 sinkOffset, float fontScale);
    void drawLevelCards(Vector2 mousePos);
    void draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed);
};
