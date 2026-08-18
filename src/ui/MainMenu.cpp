#include "MainMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <iostream>

// --------------------------------------------------------------------------
// Track names from SelectorScreen.reanim used for interactive buttons
// --------------------------------------------------------------------------
static const char* TRACK_START_ADVENTURE = "SelectorScreen_StartAdventure_button";
static const char* TRACK_SURVIVAL        = "SelectorScreen_Survival_button"; // The texture has the text "MINI-GAMES" on the tombstone button but is named "Survival"
static const char* TRACK_CHALLENGES      = "SelectorScreen_Challenges_button"; // The texture has the text "PUZZLE" on the tombstone button but is named "Challenges"
static const char* TRACK_ZEN_GARDEN      = "SelectorScreen_ZenGarden_button";

// Highlight image names (swap on hover via OverrideTrackImage)
static const char* IMG_START_ADV_HL      = "IMAGE_REANIM_SELECTORSCREEN_STARTADVENTURE_HIGHLIGHT";
static const char* IMG_SURVIVAL_HL       = "IMAGE_REANIM_SELECTORSCREEN_SURVIVAL_HIGHLIGHT";
static const char* IMG_CHALLENGES_HL     = "IMAGE_REANIM_SELECTORSCREEN_CHALLENGES_HIGHLIGHT";
static const char* IMG_ZEN_GARDEN_HL     = "IMAGE_REANIM_SELECTORSCREEN_VASEBREAKER_HIGHLIGHT";

// The reanim was designed for an 800x600 canvas. We render at native 1.0x scale.
static constexpr float REANIM_SCALE = 1.0f;

// The SelectorScreen.reanim canvas is 600px tall. It matches the 800x600 virtual
// canvas height perfectly, so no vertical offset is needed.
static float ComputeYOffset() {
    return 0.0f;
}

// Frame index in the opening animation where the main menu is fully visible
// with all tombstones, signs, and buttons in their final resting positions.
static constexpr float MENU_REST_FRAME = 40.0f;

MainMenu::MainMenu(Resources& res)
    : m_res(res), m_action(MenuAction::None), m_userDialog(res) {
    // Initialize profile manager
    ProfileManager::GetInstance().Init();

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

    // Load the Zombie_hand reanim
    std::string handReanimPath = res.GetAssetPath("assets/reanim/Zombie_hand.reanim");
    ReanimDefinition handDef = res.LoadReanim(handReanimPath);
    m_handAnim.SetResources(handDef, res);
    m_handAnim.SetFrame(0.0f);
    m_handAnim.SetPaused(true);

    // Hide sky in m_anim so that we can render the sky first, then clouds, then foreground
    m_anim.SetTrackVisible("SelectorScreen_BG", false);
    m_anim.SetTrackVisible("SelectorScreen_Adventure_button", false);
    m_anim.SetTrackVisible("SelectorScreen_Adventure_shadow", false);
    m_anim.SetTrackVisible("almanac_key_shadow", false);

    // Setup 6 concurrent animated drifting clouds (each running its own PopCap track)
    struct CloudConfig {
        const char* name;
        int startFrame;
        int endFrame;
        float initialProgress; // Stagger initial positions across screen [0..1]
        float speed;
    };

    static const CloudConfig CLOUD_CONFIGS[] = {
        { "anim_cloud1", 198, 335, 0.00f, 1.3f },
        { "anim_cloud7", 336, 421, 0.45f, 1.5f },
        { "anim_cloud2", 422, 502, 0.20f, 1.4f },
        { "anim_cloud4", 503, 568, 0.70f, 1.6f },
        { "anim_cloud6", 569, 638, 0.35f, 1.35f },
        { "anim_cloud5", 639, 705, 0.60f, 1.45f },
    };

    m_cloudAnims.reserve(6);
    for (const auto& cfg : CLOUD_CONFIGS) {
        m_cloudAnims.emplace_back();
        Reanimation& cAnim = m_cloudAnims.back();
        cAnim.SetResources(def, res);
        cAnim.AddCustomAnimation(cfg.name, cfg.startFrame, cfg.endFrame);
        cAnim.SetAnimation(cfg.name);
        cAnim.SetSpeed(cfg.speed);
        float duration = (float)(cfg.endFrame - cfg.startFrame);
        cAnim.SetFrame((float)cfg.startFrame + cfg.initialProgress * duration);
        cAnim.SetPaused(false);
    }

    // Load background sky texture
    m_bgTex = res.GetTexture("SELECTORSCREEN_BG");

    // Load bottom-bar button textures from the already-loaded resource map
    m_optionsBtn   = res.GetTexture("SELECTORSCREEN_OPTIONS1");
    m_optionsBtnHl = res.GetTexture("SELECTORSCREEN_OPTIONS2");
    m_helpBtn      = res.GetTexture("SELECTORSCREEN_HELP1");
    m_helpBtnHl    = res.GetTexture("SELECTORSCREEN_HELP2");
    m_quitBtn      = res.GetTexture("SELECTORSCREEN_QUIT1");
    m_quitBtnHl    = res.GetTexture("SELECTORSCREEN_QUIT2");
    m_storeBtn     = res.GetTexture("SELECTORSCREEN_STORE");
    m_storeBtnHl   = res.GetTexture("SELECTORSCREEN_STOREHIGHLIGHT");
    m_almanacBtn   = res.GetTexture("SELECTORSCREEN_ALMANAC");
    m_almanacBtnHl = res.GetTexture("SELECTORSCREEN_ALMANACHIGHLIGHT");
    m_almanacShadow = res.GetTexture("SELECTORSCREEN_ALMANAC_SHADOW");
    m_keyShadow     = res.GetTexture("SELECTORSCREEN_KEY_SHADOW");
    m_zenGardenBtn   = res.GetTexture("SELECTORSCREEN_ZENGARDEN");
    m_zenGardenBtnHl = res.GetTexture("SELECTORSCREEN_ZENGARDENHIGHLIGHT");

    // Load the DwarvenTodcraft24 bitmap font
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);
}

MainMenu::~MainMenu() {
    // Textures are owned by Resources singleton, no need to unload here
}

void MainMenu::update(float dt) {
    AudioManager::GetInstance().PlayMusic(MusicTrack::MainMenu);

    // Reset action each frame
    m_action = MenuAction::None;

    // If UserDialog modal popup is open, handle its updates exclusively
    if (m_userDialog.IsOpen()) {
        m_userDialog.Update(dt);
        return;
    }

    // Animate all drifting clouds concurrently
    for (auto& cAnim : m_cloudAnims) {
        cAnim.Update(dt);
    }

    // If zombie hand animation is emerging from the ground, update and wait until finished
    if (m_handActive) {
        m_anim.ClearTrackImageOverride(TRACK_START_ADVENTURE);
        m_anim.ClearTrackImageOverride(TRACK_SURVIVAL);
        m_anim.ClearTrackImageOverride(TRACK_CHALLENGES);
        m_anim.ClearTrackImageOverride(TRACK_ZEN_GARDEN);

        m_handTime += dt;
        m_handAnim.Update(dt);
        if (m_handAnim.GetCurrentFrame() >= m_handAnim.GetEndFrame() - 1 || m_handTime >= 0.83f) {
            m_handActive = false;
            m_handAnim.SetPaused(true);
            m_action = m_pendingAction;
            m_pendingAction = MenuAction::None;
        }
        return;
    }

    if (!IsUIInteractionEnabled()) return;

    Vector2 mousePos = GetVirtualMousePosition();
    float yOffset = ComputeYOffset();

    // --- WoodSign User Selector ("(if this is not you, click here)") ---
    Rectangle signRect = { 25.0f, 126.0f, 291.0f, 120.0f };
    bool signHovered = CheckCollisionPointRec(mousePos, signRect);
    if (signHovered) {
        m_anim.OverrideTrackImage("woodsign3", "IMAGE_REANIM_SELECTORSCREEN_WOODSIGN3_PRESS");
        m_anim.OverrideTrackImage("woodsign2", "IMAGE_REANIM_SELECTORSCREEN_WOODSIGN2_PRESS");
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_userDialog.Open();
            return;
        }
    } else {
        m_anim.ClearTrackImageOverride("woodsign3");
        m_anim.ClearTrackImageOverride("woodsign2");
    }

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
        { TRACK_ZEN_GARDEN,      IMG_ZEN_GARDEN_HL,   MenuAction::Vasebreaker },
    };

    for (const auto& btn : buttons) {
        Rectangle bounds = m_anim.GetTrackBounds(btn.trackName, 0, yOffset, REANIM_SCALE);
        
        std::string texName;
        if (btn.action == MenuAction::StartAdventure) texName = "SELECTORSCREEN_STARTADVENTURE_BUTTON1";
        else if (btn.action == MenuAction::Level1) texName = "SELECTORSCREEN_SURVIVAL_BUTTON";
        else if (btn.action == MenuAction::Level2) texName = "SELECTORSCREEN_CHALLENGES_BUTTON";
        else if (btn.action == MenuAction::Vasebreaker) texName = "SELECTORSCREEN_VASEBREAKER_BUTTON";

        bool hovered = (bounds.width > 0) && isGraveButtonHovered(mousePos, bounds, texName);

        if (hovered) {
            m_anim.OverrideTrackImage(btn.trackName, btn.highlightImg);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_pendingAction = btn.action;
                m_handActive = true;
                m_handTime = 0.0f;
                m_handAnim.SetFrame(0.0f);
                m_handAnim.SetPaused(false);
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/dirt_rise.ogg"));
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/evillaugh.ogg"));
            }
        } else {
            m_anim.ClearTrackImageOverride(btn.trackName);
        }
    }

    // --- Bottom-bar buttons (Options / Help / Quit) ---
    // Positioned exactly on top of the three background flower pots
    float screenW = 800.0f;
    float screenH = 600.0f;

    // Options button (left flower pot)
    float optW = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.width : 81.0f;
    float optH = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.height + 23.0f : 54.0f;
    Rectangle optRect = { 565.0f, 475.0f, optW, optH };
    if (CheckCollisionPointRec(mousePos, optRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Options;
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
    }

    // Help button (middle flower pot)
    float helpW = (m_helpBtn.id != 0) ? (float)m_helpBtn.width : 48.0f;
    float helpH = (m_helpBtn.id != 0) ? (float)m_helpBtn.height + 33.0f : 55.0f;
    Rectangle helpRect = { 647.0f, 499.0f, helpW, helpH };
    if (CheckCollisionPointRec(mousePos, helpRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Help;
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/paper.ogg"));
    }

    // Quit button (right flower pot)
    float quitW = (m_quitBtn.id != 0) ? (float)m_quitBtn.width + 10.0f : 57.0f;
    float quitH = (m_quitBtn.id != 0) ? (float)m_quitBtn.height + 10.0f : 37.0f;
    Rectangle quitRect = { 715.0f, 510.0f, quitW, quitH };
    if (CheckCollisionPointRec(mousePos, quitRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_action = MenuAction::Quit;
    }

    // Shop button (car keys hanging near right lawn)
    float storeW = (m_storeBtn.id != 0) ? (float)m_storeBtn.width : 130.0f;
    float storeH = (m_storeBtn.id != 0) ? (float)m_storeBtn.height : 89.0f;
    Rectangle storeRect = { 415.0f, 435.0f, storeW, storeH };
    if (isGraveButtonHovered(mousePos, storeRect, "SELECTORSCREEN_STORE")) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_action = MenuAction::Shop;
        }
    }

    // Almanac button (green book in middle lawn)
    float almanacW = (m_almanacBtn.id != 0) ? (float)m_almanacBtn.width : 99.0f;
    float almanacH = (m_almanacBtn.id != 0) ? (float)m_almanacBtn.height : 99.0f;
    Rectangle almanacRect = { 295.0f, 430.0f, almanacW, almanacH };
    if (isGraveButtonHovered(mousePos, almanacRect, "SELECTORSCREEN_ALMANAC")) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_action = MenuAction::Almanac;
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/paper.ogg"));
        }
    }

    // Zen Garden button (potted sprout on left lawn)
    float zenW = (m_zenGardenBtn.id != 0) ? (float)m_zenGardenBtn.width : 191.0f;
    float zenH = (m_zenGardenBtn.id != 0) ? (float)m_zenGardenBtn.height : 163.0f;
    Rectangle zenRect = { 165.0f, 415.0f, zenW, zenH };
    if (isGraveButtonHovered(mousePos, zenRect, "SELECTORSCREEN_ZENGARDEN")) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m_action = MenuAction::ZenGarden;
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
        }
    }
}

void MainMenu::draw() {
    float screenW = 800.0f;
    float screenH = 600.0f;
    float yOffset = ComputeYOffset();
    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Draw base sky background (stretched across 800x600 canvas)
    if (m_bgTex.id != 0) {
        DrawTexturePro(m_bgTex,
                       Rectangle{ 0.0f, 0.0f, (float)m_bgTex.width, (float)m_bgTex.height },
                       Rectangle{ 0.0f, 0.0f, screenW, screenH },
                       Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 2. Draw all drifting clouds in the sky (BEHIND foreground trees, grass hill, tombstones, and signs)
    for (const auto& cAnim : m_cloudAnims) {
        cAnim.Draw(0, yOffset, REANIM_SCALE);
    }

    // 3. Draw foreground elements (trees, grass hill, tombstones, buttons, leaves, flowers, signs)
    // Anchor to the bottom of the window so the grass line matches the window's bottom edge.
    m_anim.Draw(0, yOffset, REANIM_SCALE);

    // --- Overlay custom "Level 1/2/3" text on the repurposed button sprites ---
    struct LevelLabel {
        const char* trackName;
        const char* label;
    };

    LevelLabel labels[] = {
        { TRACK_START_ADVENTURE, "Levels" },
        { TRACK_SURVIVAL,        "Wall-nut Bowling" },
        { TRACK_CHALLENGES,      "Brain Busters" },
        { TRACK_ZEN_GARDEN,      "Vasebreaker" },
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

    // 4. Draw Zombie Hand emerging animation if active
    if (m_handActive) {
        m_handAnim.Draw(0, yOffset, REANIM_SCALE);
    }

    // --- Draw bottom-bar buttons (Options / Help / Quit) ---
    float optW = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.width : 81.0f;
    float optH = (m_optionsBtn.id != 0) ? (float)m_optionsBtn.height + 23.0f : 54.0f;
    Rectangle optRect = { 565.0f, 475.0f, optW, optH };

    float helpW = (m_helpBtn.id != 0) ? (float)m_helpBtn.width : 48.0f;
    float helpH = (m_helpBtn.id != 0) ? (float)m_helpBtn.height + 33.0f : 55.0f;
    Rectangle helpRect = { 647.0f, 499.0f, helpW, helpH };

    float quitW = (m_quitBtn.id != 0) ? (float)m_quitBtn.width + 10.0f : 57.0f;
    float quitH = (m_quitBtn.id != 0) ? (float)m_quitBtn.height + 10.0f : 37.0f;
    Rectangle quitRect = { 715.0f, 510.0f, quitW, quitH };

    if (!IsUIInteractionEnabled()) {
        m_anim.ClearTrackImageOverride(TRACK_START_ADVENTURE);
        m_anim.ClearTrackImageOverride(TRACK_SURVIVAL);
        m_anim.ClearTrackImageOverride(TRACK_CHALLENGES);
        m_anim.ClearTrackImageOverride(TRACK_ZEN_GARDEN);
    }

    // Draw Options button
    bool allowHover = IsUIInteractionEnabled() && !m_userDialog.IsOpen();
    if (m_optionsBtn.id != 0) {
        bool hovered = allowHover && CheckCollisionPointRec(mousePos, optRect);
        Texture2D tex = hovered ? (m_optionsBtnHl.id != 0 ? m_optionsBtnHl : m_optionsBtn) : m_optionsBtn;
        DrawTexture(tex, (int)optRect.x, (int)optRect.y + 15, WHITE);
    } else {
        DrawButton(optRect, "Options", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }

    // Draw Help button
    if (m_helpBtn.id != 0) {
        bool hovered = allowHover && CheckCollisionPointRec(mousePos, helpRect);
        Texture2D tex = hovered ? (m_helpBtnHl.id != 0 ? m_helpBtnHl : m_helpBtn) : m_helpBtn;
        DrawTexture(tex, (int)helpRect.x, (int)helpRect.y + 30, WHITE);
    } else {
        DrawButton(helpRect, "Help", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }

    // Draw Quit button
    if (m_quitBtn.id != 0) {
        bool hovered = allowHover && CheckCollisionPointRec(mousePos, quitRect);
        Texture2D tex = hovered ? (m_quitBtnHl.id != 0 ? m_quitBtnHl : m_quitBtn) : m_quitBtn;
        DrawTexture(tex, (int)quitRect.x + 5, (int)quitRect.y + 5, WHITE);
    } else {
        DrawButton(quitRect, "Quit", ColorAlpha(DARKGRAY, 0.7f), ColorAlpha(GRAY, 0.8f), WHITE);
    }

    // Draw Almanac button & shadow
    float almanacW = (m_almanacBtn.id != 0) ? (float)m_almanacBtn.width : 99.0f;
    float almanacH = (m_almanacBtn.id != 0) ? (float)m_almanacBtn.height : 99.0f;
    Rectangle almanacRect = { 295.0f, 430.0f, almanacW, almanacH };
    if (m_almanacShadow.id != 0) {
        DrawTexture(m_almanacShadow, (int)almanacRect.x - 5, (int)almanacRect.y + 20, WHITE);
    }
    if (m_almanacBtn.id != 0) {
        bool hovered = isGraveButtonHovered(mousePos, almanacRect, "SELECTORSCREEN_ALMANAC");
        Texture2D tex = hovered ? (m_almanacBtnHl.id != 0 ? m_almanacBtnHl : m_almanacBtn) : m_almanacBtn;
        DrawTexture(tex, (int)almanacRect.x, (int)almanacRect.y, WHITE);
    }

    // Draw Shop button & shadow
    float storeW = (m_storeBtn.id != 0) ? (float)m_storeBtn.width : 130.0f;
    float storeH = (m_storeBtn.id != 0) ? (float)m_storeBtn.height : 89.0f;
    Rectangle storeRect = { 415.0f, 435.0f, storeW, storeH };
    if (m_keyShadow.id != 0) {
        DrawTexture(m_keyShadow, (int)storeRect.x - 2, (int)storeRect.y - 28, WHITE);
    }
    if (m_storeBtn.id != 0) {
        bool hovered = isGraveButtonHovered(mousePos, storeRect, "SELECTORSCREEN_STORE");
        Texture2D tex = hovered ? (m_storeBtnHl.id != 0 ? m_storeBtnHl : m_storeBtn) : m_storeBtn;
        DrawTexture(tex, (int)storeRect.x, (int)storeRect.y, WHITE);
    }

    // Draw Zen Garden button
    float zenW = (m_zenGardenBtn.id != 0) ? (float)m_zenGardenBtn.width : 191.0f;
    float zenH = (m_zenGardenBtn.id != 0) ? (float)m_zenGardenBtn.height : 163.0f;
    Rectangle zenRect = { 165.0f, 415.0f, zenW, zenH };
    if (m_zenGardenBtn.id != 0) {
        bool hovered = isGraveButtonHovered(mousePos, zenRect, "SELECTORSCREEN_ZENGARDEN");
        Texture2D tex = hovered ? (m_zenGardenBtnHl.id != 0 ? m_zenGardenBtnHl : m_zenGardenBtn) : m_zenGardenBtn;
        DrawTexture(tex, (int)zenRect.x, (int)zenRect.y, WHITE);
    }

    // --- Draw current active user name on the WoodSign plaque (Green color, placed inside plaque) ---
    std::string activeUser = ProfileManager::GetInstance().GetActiveUserName();
    Rectangle userShadowRect = { 27.0f, 126.0f, 287.0f, 36.0f };
    Rectangle userTextRect   = { 25.0f, 124.0f, 287.0f, 36.0f };
    m_font.DrawTextCentered(activeUser.c_str(), userShadowRect, 0.85f, ColorAlpha(BLACK, 0.8f));
    m_font.DrawTextCentered(activeUser.c_str(), userTextRect, 0.85f, Color{ 100, 255, 100, 255 });

    // --- Draw UserDialog modal popup if open ---
    if (m_userDialog.IsOpen()) {
        m_userDialog.Draw();
    }
}

MenuAction MainMenu::getAction() const {
    return m_action;
}

void MainMenu::resetAction() {
    m_action = MenuAction::None;
}

bool MainMenu::isGraveButtonHovered(Vector2 mousePos, Rectangle bounds, const std::string& texName) {
    if (m_handActive) return false;
    if (m_userDialog.IsOpen()) return false;
    if (!IsUIInteractionEnabled()) return false;
    if (!CheckCollisionPointRec(mousePos, bounds)) return false;
    int localX = (int)((mousePos.x - bounds.x) / REANIM_SCALE);
    int localY = (int)((mousePos.y - bounds.y) / REANIM_SCALE);
    return !m_res.IsPixelTransparent(texName, localX, localY);
}
