#include "LevelSelectMenu.h"
#include "AudioManager.h"
#include "UIHelpers.h"
#include <algorithm>

LevelSelectMenu::LevelSelectMenu(Resources& res)
    : m_res(res) {
    m_skyTex         = res.GetTexture("SELECTORSCREEN_BG");
    m_bgHorizon      = res.GetTexture("MAINMENU_HORIZONEXTENDED");
    m_bgDistantTree  = res.GetTexture("MAINMENU_BACKGROUND_TREE_LEVELSELECTION");
    m_bgBush         = res.GetTexture("MAINMENU_BUSHEXTENDED");
    m_bgTex          = res.GetTexture("LEVELSELECT_BACKGROUND");
    m_bgTreeLeft     = res.GetTexture("MAINMENU_TREE_LEVELSELECTION");

    m_btnDay      = res.GetTexture("LEVELSELECT_BUTTON_DAY");
    m_btnDaySel   = res.GetTexture("LEVELSELECT_BUTTON_DAY_SELECTED");
    m_btnDayDis   = res.GetTexture("LEVELSELECT_BUTTON_DAY_DISABLED");

    m_btnNight    = res.GetTexture("LEVELSELECT_BUTTON_NIGHT");
    m_btnNightSel = res.GetTexture("LEVELSELECT_BUTTON_NIGHT_SELECTED");
    m_btnNightDis = res.GetTexture("LEVELSELECT_BUTTON_NIGHT_DISABLED");

    m_btnPoolDis   = res.GetTexture("LEVELSELECT_BUTTON_POOL_DISABLED");
    m_btnFogDis    = res.GetTexture("LEVELSELECT_BUTTON_FOG_DISABLED");
    m_btnRoofDis   = res.GetTexture("LEVELSELECT_BUTTON_ROOF_DISABLED");

    m_panelFrame      = res.GetTexture("LEVELSELECT_LEVELPANELFRAME");
    m_specialFrame    = res.GetTexture("LEVELSELECT_LEVELPANELSPECIALFRAME");
    m_specialFrameRip = res.GetTexture("LEVELSELECT_LEVELPANELSPECIALFRAME_RIP");
    m_cloudyFrame     = res.GetTexture("LEVELSELECT_LEVELPANELCLOUDYFRAME");
    m_groundDay       = res.GetTexture("ALMANAC_GROUNDDAY");
    m_groundNight     = res.GetTexture("ALMANAC_GROUNDNIGHT");

    m_btnIndex    = res.GetTexture("ALMANAC_INDEXBUTTON");
    m_btnIndexHl  = res.GetTexture("ALMANAC_INDEXBUTTONHIGHLIGHT");

    m_btnLeft       = res.GetTexture("BUTTON_LEFT");
    m_btnMiddle     = res.GetTexture("BUTTON_MIDDLE");
    m_btnRight      = res.GetTexture("BUTTON_RIGHT");
    m_btnLeftDown   = res.GetTexture("BUTTON_DOWN_LEFT");
    m_btnMiddleDown = res.GetTexture("BUTTON_DOWN_MIDDLE");
    m_btnRightDown  = res.GetTexture("BUTTON_DOWN_RIGHT");

    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    if (FileExists(fontPng.c_str()) && FileExists(fontTxt.c_str())) {
        m_fontLoaded = m_font.Load(fontPng, fontTxt);
    }

    std::string briannePng = res.GetAssetPath("assets/data/_BrianneTod16.png");
    std::string brianneTxt = res.GetAssetPath("assets/data/BrianneTod16.txt");
    if (FileExists(briannePng.c_str()) && FileExists(brianneTxt.c_str())) {
        m_brianneLoaded = m_brianneFont.Load(briannePng, brianneTxt);
    }
}

bool LevelSelectMenu::isStageHovered(Vector2 mousePos, Rectangle screenRect, const std::string& texName, float scale) {
    if (!CheckCollisionPointRec(mousePos, screenRect)) return false;
    int localX = (int)((mousePos.x - screenRect.x) / scale);
    int localY = (int)((mousePos.y - screenRect.y) / scale);
    return !m_res.IsPixelTransparent(texName, localX, localY);
}

void LevelSelectMenu::update(float dt, bool& showLevelSelect) {
    m_action = LevelSelectAction::None;
    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Back navigation (ESC key or Back button)
    if (IsKeyPressed(KEY_ESCAPE)) {
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/paper.ogg"));
        showLevelSelect = false;
        return;
    }

    Rectangle backBtnRect = { 15.0f, 554.0f, 115.0f, 26.0f };
    if (CheckCollisionPointRec(mousePos, backBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
        showLevelSelect = false;
        return;
    }

    // 2. Stage Tombstone Button Click Checks
    float bgScale = 800.0f / 3940.0f;
    float btnScale = bgScale * 0.85f * 1.5f;
    Rectangle dayRect   = { 25.0f,  380.0f, 594.0f * btnScale, 657.0f * btnScale };
    Rectangle nightRect = { 170.0f, 410.0f, 545.0f * btnScale, 481.0f * btnScale };

    bool dayHovered   = isStageHovered(mousePos, dayRect, "LEVELSELECT_BUTTON_DAY", btnScale);
    bool nightHovered = isStageHovered(mousePos, nightRect, "LEVELSELECT_BUTTON_NIGHT", btnScale);

    if (dayHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (m_stageMode != LevelStageMode::Day) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            m_stageMode = LevelStageMode::Day;
        }
        return;
    }

    if (nightHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (m_stageMode != LevelStageMode::Night) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            m_stageMode = LevelStageMode::Night;
        }
        return;
    }

    // 3. Level 1 Play Button Interaction (Only entering level when clicking PLAY button)
    if (m_stageMode == LevelStageMode::Day) {
        float cardW = 215.0f;
        float specW = 100.8f;
        float specH = 54.0f;
        float overlap = 28.7f;
        float startX = 38.0f;
        float cardY = 30.0f;

        float btnW = 118.0f;
        float btnH = 26.0f;
        float btnX = startX + (cardW - btnW) / 2.0f;
        float btnY = cardY + specH - overlap + 139.0f;
        Rectangle playBtnRect = { btnX, btnY, btnW, btnH };

        if (CheckCollisionPointRec(mousePos, playBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            m_action = LevelSelectAction::PlayLevel1;
            showLevelSelect = false;
            return;
        }
    }
}

void LevelSelectMenu::drawTombstoneButton(
    const std::string& label,
    Rectangle screenRect,
    Texture2D normalTex,
    Texture2D selTex,
    bool hovered,
    bool selected,
    bool enabled,
    Vector2 nativeCenter,
    float angleDeg,
    Vector2 sinkOffset,
    float fontScale
) {
    Texture2D texToDraw = (selected && selTex.id != 0) ? selTex : normalTex;
    if (texToDraw.id != 0) {
        DrawTexturePro(
            texToDraw,
            Rectangle{ 0.0f, 0.0f, (float)texToDraw.width, (float)texToDraw.height },
            screenRect,
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    if (m_fontLoaded && !label.empty() && normalTex.width > 0) {
        float btnScale = screenRect.width / (float)normalTex.width;
        Vector2 center = {
            screenRect.x + nativeCenter.x * btnScale,
            screenRect.y + nativeCenter.y * btnScale
        };

        if (selected) {
            center.x += sinkOffset.x * btnScale;
            center.y += sinkOffset.y * btnScale;
        }

        Vector2 shadowCenter = { center.x + 1.5f, center.y + 1.5f };
        Color textColor;
        if (!enabled) {
            // Disabled -> Keep the same grey color
            textColor = Color{ 140, 140, 140, 255 };
        } else if (selected) {
            // Selected -> The same green color
            textColor = GREEN;
        } else if (hovered) {
            // Hovered -> The same yellow color
            textColor = Color{ 255, 230, 80, 255 };
        } else {
            // Not selected -> Change to a darker grey
            textColor = Color{ 95, 95, 95, 255 };
        }

        m_font.DrawTextRotated(label.c_str(), shadowCenter, angleDeg, fontScale, ColorAlpha(BLACK, 0.8f));
        m_font.DrawTextRotated(label.c_str(), center, angleDeg, fontScale, textColor);
    }
}

void LevelSelectMenu::drawLevelCards(Vector2 mousePos) {
    struct LevelCardInfo {
        std::string name;
        bool active;
    };

    std::vector<LevelCardInfo> cards;
    if (m_stageMode == LevelStageMode::Day) {
        cards = { { "Level 1", true }, { "Level 2", false }, { "Level 3", false } };
    } else {
        cards = { { "Level 4", false }, { "Level 5", false }, { "Level 6", false } };
    }

    float cardW = 215.0f;
    float specW = 100.8f;
    float specH = 54.0f;
    float panH = cardW * (895.0f / 1118.0f);  // 172.1f
    float overlap = 28.7f;
    float dx = (cardW - specW) / 2.0f;        // 57.1f
    float cardY = 30.0f;
    float startX = 38.0f;
    float spacing = 254.5f;

    for (size_t i = 0; i < cards.size(); ++i) {
        float cardX = startX + (float)i * spacing;
        Color tint = cards[i].active ? WHITE : Color{ 160, 160, 160, 255 };

        // 1. Draw Ground Texture in rectangular upper window (Layer 1)
        Texture2D groundTex = (m_stageMode == LevelStageMode::Day) ? m_groundDay : m_groundNight;
        if (groundTex.id != 0) {
            float holeX = 120.0f * (cardW / 1118.0f);
            float holeY = 118.0f * (cardW / 1118.0f);
            float holeW = 872.0f * (cardW / 1118.0f);
            float holeH = 464.0f * (cardW / 1118.0f);
            Rectangle groundRect = {
                cardX + holeX - 1.0f,
                cardY + specH - overlap + holeY - 1.0f,
                holeW + 2.0f,
                holeH + 2.0f
            };
            DrawTexturePro(
                groundTex,
                Rectangle{ 0.0f, 0.0f, (float)groundTex.width, (float)groundTex.height },
                groundRect,
                Vector2{ 0.0f, 0.0f },
                0.0f,
                tint
            );
        }

        // 2. Draw LevelSelect_LevelPanelFrame.png (Base Body - Layer 2)
        Rectangle panelRect = { cardX, cardY + specH - overlap, cardW, panH };
        if (m_panelFrame.id != 0) {
            DrawTexturePro(
                m_panelFrame,
                Rectangle{ 0.0f, 0.0f, (float)m_panelFrame.width, (float)m_panelFrame.height },
                panelRect,
                Vector2{ 0.0f, 0.0f },
                0.0f,
                tint
            );
        }

        // 3. Draw LevelSelect_LevelPanelSpecialFrame(_Rip).png (Top Cap connected seamlessly)
        Rectangle specialRect = { cardX + dx, cardY, specW, specH };
        Texture2D topCapTex = cards[i].active ? m_specialFrame : m_specialFrameRip;
        if (topCapTex.id != 0) {
            DrawTexturePro(
                topCapTex,
                Rectangle{ 0.0f, 0.0f, (float)topCapTex.width, (float)topCapTex.height },
                specialRect,
                Vector2{ 0.0f, 0.0f },
                0.0f,
                tint
            );
        }

        // 3. Level Title inside bottom bar (Alignment C2 - larger and lower)
        Rectangle textRect = { cardX, cardY + specH - overlap + 117.0f, cardW, 22.0f };
        Rectangle shadowRect = { textRect.x + 1.5f, textRect.y + 1.5f, textRect.width, textRect.height };
        Color textColor = cards[i].active ? WHITE : Color{ 140, 140, 140, 255 };

        if (m_brianneLoaded) {
            m_brianneFont.DrawTextCentered(cards[i].name.c_str(), shadowRect, 0.95f, ColorAlpha(BLACK, 0.85f));
            m_brianneFont.DrawTextCentered(cards[i].name.c_str(), textRect, 0.95f, textColor);
        } else if (m_fontLoaded) {
            m_font.DrawTextCentered(cards[i].name.c_str(), shadowRect, 0.75f, ColorAlpha(BLACK, 0.85f));
            m_font.DrawTextCentered(cards[i].name.c_str(), textRect, 0.75f, textColor);
        }

        // 4. Action Button ("PLAY" or "LOCKED")
        if (cards[i].active) {
            float btnW = 118.0f;
            float btnH = 26.0f;
            float btnX = cardX + (cardW - btnW) / 2.0f;
            float btnY = cardY + specH - overlap + 139.0f;
            Rectangle playRect = { btnX, btnY, btnW, btnH };
            bool playHover = CheckCollisionPointRec(mousePos, playRect);
            bool playPressed = playHover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
            draw3SliceButton(playRect, "PLAY", playHover, playPressed);
        } else {
            Rectangle lockedRect = { cardX, cardY + specH - overlap + 142.0f, cardW, 20.0f };
            if (m_fontLoaded) {
                m_font.DrawTextCentered("LOCKED", lockedRect, 0.55f, Color{ 180, 80, 80, 255 });
            }
        }
    }
}

void LevelSelectMenu::draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed) {
    Texture2D leftTex   = pressed ? m_btnLeftDown : m_btnLeft;
    Texture2D midTex    = pressed ? m_btnMiddleDown : m_btnMiddle;
    Texture2D rightTex  = pressed ? m_btnRightDown : m_btnRight;

    if (leftTex.id != 0 && midTex.id != 0 && rightTex.id != 0) {
        float scaleY = rect.height / (float)leftTex.height;
        float lw = (float)leftTex.width * scaleY;
        float rw = (float)rightTex.width * scaleY;
        float mw = rect.width - lw - rw;

        // Draw left side piece
        DrawTexturePro(
            leftTex,
            Rectangle{ 0.0f, 0.0f, (float)leftTex.width, (float)leftTex.height },
            Rectangle{ rect.x, rect.y, lw, rect.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        // Draw middle piece (stretched to fill the remaining width)
        DrawTexturePro(
            midTex,
            Rectangle{ 0.0f, 0.0f, (float)midTex.width, (float)midTex.height },
            Rectangle{ rect.x + lw, rect.y, mw, rect.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        // Draw right side piece
        DrawTexturePro(
            rightTex,
            Rectangle{ 0.0f, 0.0f, (float)rightTex.width, (float)rightTex.height },
            Rectangle{ rect.x + lw + mw, rect.y, rw, rect.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        // Fallback solid color rendering
        DrawRectangleRec(rect, pressed ? DARKGRAY : (hovered ? GRAY : LIGHTGRAY));
        DrawRectangleLinesEx(rect, 2.0f, BLACK);
    }

    // Render button text centered
    Rectangle textRect = rect;
    if (pressed) {
        textRect.y += 2.0f; // Visual displacement when clicked
    }
    Color textColor = hovered ? GREEN : Color{ 220, 180, 80, 255 };
    if (m_fontLoaded) {
        m_font.DrawTextCentered(text, textRect, 0.65f, textColor);
    }
}

void LevelSelectMenu::draw() {
    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Draw Background Sky (800x600)
    if (m_skyTex.id != 0) {
        DrawTexturePro(
            m_skyTex,
            Rectangle{ 0.0f, 0.0f, (float)m_skyTex.width, (float)m_skyTex.height },
            Rectangle{ 0.0f, 0.0f, 800.0f, 600.0f },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        ClearBackground(SKYBLUE);
    }

    // 2. Draw Horizon Extended (MainMenu_HorizonExtended.png) starting from x=900, scaled to window height
    if (m_bgHorizon.id != 0) {
        float horizonScale = 600.0f / (float)m_bgHorizon.height;
        float srcW = 800.0f / horizonScale;
        DrawTexturePro(
            m_bgHorizon,
            Rectangle{ 900.0f, 0.0f, srcW, (float)m_bgHorizon.height },
            Rectangle{ 0.0f, 0.0f, 800.0f, 600.0f },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 3. Draw Big Tree on Left (MainMenu_Tree_LevelSelection.png) touching top and bottom of screen, behind all other scenery
    if (m_bgTreeLeft.id != 0) {
        float treeScale = 600.0f / (float)m_bgTreeLeft.height;
        float treeW = (float)m_bgTreeLeft.width * treeScale;
        DrawTexturePro(
            m_bgTreeLeft,
            Rectangle{ 0.0f, 0.0f, (float)m_bgTreeLeft.width, (float)m_bgTreeLeft.height },
            Rectangle{ 0.0f, 0.0f, treeW, 600.0f },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 4. Draw Distant Pine Trees on Right Horizon (MainMenu_Background_Tree_LevelSelection.png)
    if (m_bgDistantTree.id != 0) {
        float treeW = (float)m_bgDistantTree.width * 0.50f;
        float treeH = (float)m_bgDistantTree.height * 0.50f;
        DrawTexturePro(
            m_bgDistantTree,
            Rectangle{ 0.0f, 0.0f, (float)m_bgDistantTree.width, (float)m_bgDistantTree.height },
            Rectangle{ 520.0f, 190.0f, treeW, treeH },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 5. Draw Extended Bush Foliage behind Stone Wall (MainMenu_BushExtended.png)
    if (m_bgBush.id != 0) {
        float bushH = (float)m_bgBush.height * (800.0f / (float)m_bgBush.width);
        DrawTexturePro(
            m_bgBush,
            Rectangle{ 0.0f, 0.0f, (float)m_bgBush.width, (float)m_bgBush.height },
            Rectangle{ 0.0f, 250.0f, 800.0f, bushH },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 6. Draw Foreground Stone Wall & Lawn (Scale by 5368 / 3940, anchored at bottom)
    float bgScale = 800.0f / 3940.0f; // 0.203045685f
    float bgW = 5368.0f * bgScale;    // 1089.95f
    float bgH = 1640.0f * bgScale;    // 332.995f
    float bgX = (800.0f - bgW) / 2.0f; // -144.97f (horizontally centered)
    float bgY = 600.0f - bgH;          // 267.0f (anchored at bottom)

    if (m_bgTex.id != 0) {
        DrawTexturePro(
            m_bgTex,
            Rectangle{ 0.0f, 0.0f, (float)m_bgTex.width, (float)m_bgTex.height },
            Rectangle{ bgX, bgY, bgW, bgH },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 6. Draw 5 Tombstone Buttons along the lawn (1.5x scale)
    float btnScale = bgScale * 0.85f * 1.5f;
    Rectangle dayRect   = { 25.0f,  380.0f, 594.0f * btnScale, 657.0f * btnScale };
    Rectangle nightRect = { 170.0f, 410.0f, 545.0f * btnScale, 481.0f * btnScale };
    Rectangle poolRect  = { 320.0f, 380.0f, 529.0f * btnScale, 488.0f * btnScale };
    Rectangle fogRect   = { 470.0f, 345.0f, 497.0f * btnScale, 653.0f * btnScale };
    Rectangle roofRect  = { 620.0f, 390.0f, 516.0f * btnScale, 599.0f * btnScale };

    bool dayHovered   = isStageHovered(mousePos, dayRect, "LEVELSELECT_BUTTON_DAY", btnScale);
    bool nightHovered = isStageHovered(mousePos, nightRect, "LEVELSELECT_BUTTON_NIGHT", btnScale);

    bool daySelected   = (m_stageMode == LevelStageMode::Day);
    bool nightSelected = (m_stageMode == LevelStageMode::Night);

    // Day (Active / Unlocked)
    drawTombstoneButton("DAY", dayRect, m_btnDay, m_btnDaySel, dayHovered, daySelected, true, Vector2{ 320.0f, 246.0f }, -3.0f, Vector2{ 2.0f, 4.0f }, 0.90f);

    // Night (Active / Unlocked)
    drawTombstoneButton("NIGHT", nightRect, m_btnNight, m_btnNightSel, nightHovered, nightSelected, true, Vector2{ 315.0f, 318.0f }, 9.0f, Vector2{ 1.0f, 4.0f }, 0.825f);

    // Pool, Fog, Roof (Disabled)
    drawTombstoneButton("POOL", poolRect, m_btnPoolDis, m_btnPoolDis, false, false, false, Vector2{ 220.0f, 252.0f }, 0.0f, Vector2{ 0.0f, 4.0f }, 0.825f);
    drawTombstoneButton("FOG", fogRect, m_btnFogDis, m_btnFogDis, false, false, false, Vector2{ 210.0f, 470.0f }, -7.0f, Vector2{ -1.0f, 4.0f }, 0.825f);
    drawTombstoneButton("ROOF", roofRect, m_btnRoofDis, m_btnRoofDis, false, false, false, Vector2{ 215.0f, 275.0f }, -9.5f, Vector2{ 1.0f, 4.0f }, 0.825f);

    // 7. Draw Upper Level Cards on the Stone Wall (Seamless Composite of LevelPanelFrame + SpecialFrame)
    drawLevelCards(mousePos);

    // 8. Draw "ADVENTURE" Carved Title near bottom edge
    Rectangle titleRect = { 200.0f, 550.0f, 400.0f, 35.0f };
    Rectangle shadowTitle = { titleRect.x + 2.0f, titleRect.y + 2.0f, titleRect.width, titleRect.height };
    if (m_fontLoaded) {
        m_font.DrawTextCentered("ADVENTURE", shadowTitle, 1.0f, ColorAlpha(BLACK, 0.8f));
        m_font.DrawTextCentered("ADVENTURE", titleRect, 1.0f, Color{ 140, 75, 20, 255 });
    }

    // 9. Draw Back Button at Bottom Left
    Rectangle backBtnRect = { 15.0f, 554.0f, 115.0f, 26.0f };
    bool backHover = CheckCollisionPointRec(mousePos, backBtnRect);
    Texture2D backTex = (backHover && m_btnIndexHl.id != 0) ? m_btnIndexHl : m_btnIndex;
    if (backTex.id != 0) {
        DrawTexturePro(
            backTex,
            Rectangle{ 0.0f, 0.0f, (float)backTex.width, (float)backTex.height },
            backBtnRect,
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
    if (m_fontLoaded) {
        m_font.DrawTextCentered("MAIN MENU", backBtnRect, 0.55f, backHover ? GREEN : Color{ 230, 210, 160, 255 });
    }
}
