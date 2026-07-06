#include "MainMenu.h"
#include "UIHelpers.h"
#include <iostream>

// --------------------------------------------------------------------------
// Track names from SelectorScreen.reanim used for interactive buttons
// --------------------------------------------------------------------------
static const char* TRACK_START_ADVENTURE = "SelectorScreen_StartAdventure_button";
static const char* TRACK_SURVIVAL        = "SelectorScreen_Survival_button";
static const char* TRACK_CHALLENGES      = "SelectorScreen_Challenges_button";
static const char* TRACK_ZEN_GARDEN      = "SelectorScreen_ZenGarden_button";

// Highlight image names (swap on hover via OverrideTrackImage)
static const char* IMG_START_ADV_HL      = "IMAGE_REANIM_SELECTORSCREEN_STARTADVENTURE_HIGHLIGHT";
static const char* IMG_SURVIVAL_HL       = "IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT";
static const char* IMG_CHALLENGES_HL     = "IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT";
static const char* IMG_ZEN_GARDEN_HL     = "IMAGE_REANIM_SELECTORSCREEN_ZENGARDENHIGHLIGHT";

// The reanim was designed for an 800x600 canvas. We scale to fill 1280x768.
static constexpr float REANIM_SCALE = 1.6f;

// The SelectorScreen.reanim canvas is 600px tall. After scaling (1.6×) it
// becomes 960px. We anchor the bottom of the reanim to the bottom of the
// window so the grass/foreground line up correctly.
static float ComputeYOffset() {
    return 768.0f - 600.0f * REANIM_SCALE;
}

// Frame index in the opening animation where the main menu is fully visible
// with all tombstones, signs, and buttons in their final resting positions.
static constexpr float MENU_REST_FRAME = 40.0f;

MainMenu::MainMenu(Resources& res)
    : m_res(res), m_action(MenuAction::None) {
    // Load the SelectorScreen reanim
    std::string reanimPath = res.GetAssetPath("assets/reanim/SelectorScreen.reanim");
    ReanimDefinition def = res.LoadReanim(reanimPath);
    m_anim.SetResources(def, res);

    // Use the opening animation and freeze at the rest frame (frame 40).
    // The "anim_idle" range intentionally hides tombstones after ~frame 40,
    // so we must NOT loop it. Instead, jump to the exact frame where every
    // element is visible and pause.
    m_anim.SetAnimation("anim_open");
    m_anim.SetFrame(MENU_REST_FRAME);
    m_anim.SetPaused(true);

    // Load bottom-bar button textures from the already-loaded resource map
    m_optionsBtn   = res.GetTexture("SELECTORSCREEN_OPTIONS1");
    m_optionsBtnHl = res.GetTexture("SELECTORSCREEN_OPTIONS2");
    m_helpBtn      = res.GetTexture("SELECTORSCREEN_HELP1");
    m_helpBtnHl    = res.GetTexture("SELECTORSCREEN_HELP2");
    m_quitBtn      = res.GetTexture("SELECTORSCREEN_QUIT1");
    m_quitBtnHl    = res.GetTexture("SELECTORSCREEN_QUIT2");

    // Load the DwarvenTodcraft24 bitmap font
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);
}

MainMenu::~MainMenu() {
    // Textures are owned by Resources singleton, no need to unload here
}

void MainMenu::update(float dt) {
    // Reset action each frame
    m_action = MenuAction::None;

    // Animation is frozen; no need to call Update.
    // m_anim.Update(dt);

    Vector2 mousePos = GetVirtualMousePosition();
    float yOffset = ComputeYOffset();

    // --- Interactive reanim-based buttons ---
    struct ButtonInfo {
        const char* trackName;
        const char* highlightImg;
        MenuAction action;
    };

    ButtonInfo buttons[] = {
        { TRACK_START_ADVENTURE, IMG_START_ADV_HL,   MenuAction::StartAdventure },
        { TRACK_SURVIVAL,        IMG_SURVIVAL_HL,     MenuAction::Level1 },
        { TRACK_CHALLENGES,      IMG_CHALLENGES_HL,   MenuAction::Level2 },
        { TRACK_ZEN_GARDEN,      IMG_ZEN_GARDEN_HL,   MenuAction::Level3 },
    };

    for (const auto& btn : buttons) {
        Rectangle bounds = m_anim.GetTrackBounds(btn.trackName, 0, yOffset, REANIM_SCALE);
        bool hovered = (bounds.width > 0) && CheckCollisionPointRec(mousePos, bounds);

        if (hovered) {
            m_anim.OverrideTrackImage(btn.trackName, btn.highlightImg);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_action = btn.action;
            }
        } else {
            m_anim.ClearTrackImageOverride(btn.trackName);
        }
    }

    // --- Bottom-bar buttons (Options / Help / Quit) ---
    // These are positioned manually at the bottom of the screen
    float screenW = 1280.0f;
    float screenH = 768.0f;

    // Options button — bottom-left area
    float optW = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.width : 60.0f;
    float optH = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.height : 30.0f;
    Rectangle optRect = { screenW - optW - 135.0f - 10.0f, screenH - optH - 10.0f, optW, optH };
    if (CheckCollisionPointRec(mousePos, optRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Options;
    }

    // Help button — next to Options
    float helpW = (m_helpBtn.id != 0) ? (float)m_helpBtn.width : 40.0f;
    float helpH = (m_helpBtn.id != 0) ? (float)m_helpBtn.height : 30.0f;
    Rectangle helpRect = { screenW - helpW - 70.0f - 10.0f, screenH - helpH - 10.0f, helpW, helpH };
    if (CheckCollisionPointRec(mousePos, helpRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Help;
    }

    // Quit button — bottom-right corner
    float quitW = (m_quitBtn.id != 0) ? (float)m_quitBtn.width : 50.0f;
    float quitH = (m_quitBtn.id != 0) ? (float)m_quitBtn.height : 30.0f;
    Rectangle quitRect = { screenW - quitW - 10.0f, screenH - quitH - 10.0f, quitW, quitH };
    if (CheckCollisionPointRec(mousePos, quitRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Quit;
    }
}

void MainMenu::draw() {
    float screenW = 1280.0f;
    float screenH = 768.0f;
    float yOffset = ComputeYOffset();
    Vector2 mousePos = GetVirtualMousePosition();

    // Draw the entire SelectorScreen reanim (background, clouds, buttons, leaves, flowers, signs)
    // Anchor to the bottom of the window so the grass line matches the window's bottom edge.
    m_anim.Draw(0, yOffset, REANIM_SCALE);

    // --- Overlay custom "Level 1/2/3" text on the repurposed button sprites ---
    struct LevelLabel {
        const char* trackName;
        const char* label;
    };

    LevelLabel labels[] = {
        { TRACK_SURVIVAL,   "Level 1" },
        { TRACK_CHALLENGES, "Level 2" },
        { TRACK_ZEN_GARDEN, "Level 3" },
    };

    for (const auto& lbl : labels) {
        Rectangle bounds = m_anim.GetTrackBounds(lbl.trackName, 0, yOffset, REANIM_SCALE);
        if (bounds.width > 0 && bounds.height > 0) {
            // Shadow pass (offset by 2px)
            Rectangle shadowBounds = { bounds.x + 2, bounds.y + 2, bounds.width, bounds.height };
            m_font.DrawTextCentered(lbl.label, shadowBounds, REANIM_SCALE, ColorAlpha(BLACK, 0.6f));
            // Main pass
            m_font.DrawTextCentered(lbl.label, bounds, REANIM_SCALE, WHITE);
        }
    }

    // --- Draw bottom-bar buttons (Options / Help / Quit) ---
    float optW = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.width : 60.0f;
    float optH = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.height : 30.0f;
    Rectangle optRect = { screenW - optW - 135.0f - 10.0f, screenH - optH - 10.0f, optW, optH };

    float helpW = (m_helpBtn.id != 0) ? (float)m_helpBtn.width : 40.0f;
    float helpH = (m_helpBtn.id != 0) ? (float)m_helpBtn.height : 30.0f;
    Rectangle helpRect = { screenW - helpW - 70.0f - 10.0f, screenH - helpH - 10.0f, helpW, helpH };

    float quitW = (m_quitBtn.id != 0) ? (float)m_quitBtn.width : 50.0f;
    float quitH = (m_quitBtn.id != 0) ? (float)m_quitBtn.height : 30.0f;
    Rectangle quitRect = { screenW - quitW - 10.0f, screenH - quitH - 10.0f, quitW, quitH };

    // Options
    if (m_optionsBtn.id != 0) {
        bool hovered = CheckCollisionPointRec(mousePos, optRect);
        Texture2D tex = hovered ? (m_optionsBtnHl.id != 0 ? m_optionsBtnHl : m_optionsBtn) : m_optionsBtn;
        DrawTexture(tex, (int)optRect.x, (int)optRect.y, WHITE);
    } else {
        DrawButton(optRect, "Options", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }

    // Help
    if (m_helpBtn.id != 0) {
        bool hovered = CheckCollisionPointRec(mousePos, helpRect);
        Texture2D tex = hovered ? (m_helpBtnHl.id != 0 ? m_helpBtnHl : m_helpBtn) : m_helpBtn;
        DrawTexture(tex, (int)helpRect.x, (int)helpRect.y, WHITE);
    } else {
        DrawButton(helpRect, "Help", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }

    // Quit
    if (m_quitBtn.id != 0) {
        bool hovered = CheckCollisionPointRec(mousePos, quitRect);
        Texture2D tex = hovered ? (m_quitBtnHl.id != 0 ? m_quitBtnHl : m_quitBtn) : m_quitBtn;
        DrawTexture(tex, (int)quitRect.x, (int)quitRect.y, WHITE);
    } else {
        DrawButton(quitRect, "Quit", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }
}

MenuAction MainMenu::getAction() const {
    return m_action;
}

void MainMenu::resetAction() {
    m_action = MenuAction::None;
}
