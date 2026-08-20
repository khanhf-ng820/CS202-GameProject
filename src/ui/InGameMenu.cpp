#include "InGameMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <algorithm>

InGameMenu::InGameMenu(Resources& res)
    : m_res(res) {
    // Load bitmap font
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);

    // Load textures
    m_menuBack        = res.GetTexture("OPTIONS_MENUBACK");
    if (m_menuBack.id == 0) m_menuBack = res.GetTexture("OPTIONS_MENUBACK_");
    if (m_menuBack.id == 0) {
        std::string path = res.GetAssetPath("assets/images/options_menuback.jpg");
        res.LoadFile(path);
        m_menuBack = res.GetTexture("OPTIONS_MENUBACK");
    }

    m_backToGameBtn   = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON0");
    if (m_backToGameBtn.id == 0) {
        std::string path = res.GetAssetPath("assets/images/options_backtogamebutton0.png");
        res.LoadFile(path);
        m_backToGameBtn = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON0");
    }

    m_backToGameBtnHl = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON2");
    if (m_backToGameBtnHl.id == 0) {
        std::string path = res.GetAssetPath("assets/images/options_backtogamebutton2.png");
        res.LoadFile(path);
        m_backToGameBtnHl = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON2");
    }

    m_sliderSlot      = res.GetTexture("OPTIONS_SLIDERSLOT");
    if (m_sliderSlot.id == 0) {
        std::string path = res.GetAssetPath("assets/images/options_sliderslot.png");
        res.LoadFile(path);
        m_sliderSlot = res.GetTexture("OPTIONS_SLIDERSLOT");
    }

    m_sliderKnob      = res.GetTexture("OPTIONS_SLIDERKNOB2");
    if (m_sliderKnob.id == 0) {
        std::string path = res.GetAssetPath("assets/images/options_sliderknob2.png");
        res.LoadFile(path);
        m_sliderKnob = res.GetTexture("OPTIONS_SLIDERKNOB2");
    }

    m_btnLeft         = res.GetTexture("BUTTON_LEFT");
    m_btnMiddle       = res.GetTexture("BUTTON_MIDDLE");
    m_btnRight        = res.GetTexture("BUTTON_RIGHT");
    m_btnLeftDown     = res.GetTexture("BUTTON_DOWN_LEFT");
    m_btnMiddleDown   = res.GetTexture("BUTTON_DOWN_MIDDLE");
    m_btnRightDown    = res.GetTexture("BUTTON_DOWN_RIGHT");
}

void InGameMenu::open() {
    m_isOpen = true;
    m_isDraggingMusicSlider = false;
    m_isDraggingSfxSlider = false;
    AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
}

void InGameMenu::close() {
    m_isOpen = false;
    m_isDraggingMusicSlider = false;
    m_isDraggingSfxSlider = false;
    AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
}

void InGameMenu::toggle() {
    if (m_isOpen) {
        close();
    } else {
        open();
    }
}

InGameMenuAction InGameMenu::update(float dt) {
    if (!m_isOpen) return InGameMenuAction::None;

    std::string sfxPath = m_res.GetAssetPath("assets/sounds/gravebutton.ogg");

    if (IsKeyPressed(KEY_ESCAPE)) {
        close();
        return InGameMenuAction::Resume;
    }

    Vector2 mousePos = GetVirtualMousePosition();

    float dialogX = (800.0f - 423.0f) / 2.0f; // 188.5f
    float dialogY = (600.0f - 498.0f) / 2.0f; // 51.0f

    float slotW = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.width : 135.0f;
    float knobW = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.width : 22.0f;
    float travelW = slotW - knobW;

    // 1. Music Volume Slider dragging
    float musicSlotX = dialogX + 205.0f;
    float musicSlotY = dialogY + 145.0f;
    Rectangle musicHitbox = { musicSlotX - 10.0f, musicSlotY - 15.0f, slotW + 20.0f, 40.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, musicHitbox)) {
        m_isDraggingMusicSlider = true;
        AudioManager::GetInstance().PlaySoundEffect(sfxPath);
    }
    if (m_isDraggingMusicSlider && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        m_isDraggingMusicSlider = false;
    }
    if (m_isDraggingMusicSlider) {
        float clampedX = std::clamp(mousePos.x - musicSlotX - knobW / 2.0f, 0.0f, travelW);
        float newVol = (travelW > 0.0f) ? (clampedX / travelW) : 1.0f;
        AudioManager::GetInstance().SetMusicVolume(newVol);
    }

    // 2. Sound FX Volume Slider dragging
    float sfxSlotX = dialogX + 205.0f;
    float sfxSlotY = dialogY + 190.0f;
    Rectangle sfxHitbox = { sfxSlotX - 10.0f, sfxSlotY - 15.0f, slotW + 20.0f, 40.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, sfxHitbox)) {
        m_isDraggingSfxSlider = true;
        AudioManager::GetInstance().PlaySoundEffect(sfxPath);
    }
    if (m_isDraggingSfxSlider && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        m_isDraggingSfxSlider = false;
    }
    if (m_isDraggingSfxSlider) {
        float clampedX = std::clamp(mousePos.x - sfxSlotX - knobW / 2.0f, 0.0f, travelW);
        float newVol = (travelW > 0.0f) ? (clampedX / travelW) : 1.0f;
        AudioManager::GetInstance().SetSoundVolume(newVol);
    }

    // 3. Restart Level button
    Rectangle restartRect = { dialogX + 91.5f, dialogY + 278.0f, 240.0f, 44.0f };
    if (CheckCollisionPointRec(mousePos, restartRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        close();
        return InGameMenuAction::RestartLevel;
    }

    // 4. Main Menu button
    Rectangle mainMenuRect = { dialogX + 91.5f, dialogY + 330.0f, 240.0f, 44.0f };
    if (CheckCollisionPointRec(mousePos, mainMenuRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        close();
        return InGameMenuAction::MainMenu;
    }

    // 5. BACK TO GAME button
    Rectangle backRect = { dialogX + 31.5f, dialogY + 385.0f, 360.0f, 100.0f };
    if (CheckCollisionPointRec(mousePos, backRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        close();
        return InGameMenuAction::Resume;
    }

    return InGameMenuAction::None;
}

void InGameMenu::draw() {
    if (!m_isOpen) return;

    Vector2 mousePos = GetVirtualMousePosition();
    float dialogX = (800.0f - 423.0f) / 2.0f; // 188.5f
    float dialogY = (600.0f - 498.0f) / 2.0f; // 51.0f

    // Dim background overlay
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.55f));

    // Draw tombstone backdrop
    if (m_menuBack.id != 0) {
        DrawTexture(m_menuBack, (int)dialogX, (int)dialogY, WHITE);
    } else {
        DrawRectangleRec({ dialogX, dialogY, 423.0f, 498.0f }, ColorAlpha(DARKGRAY, 0.95f));
        DrawRectangleLinesEx({ dialogX, dialogY, 423.0f, 498.0f }, 4.0f, BROWN);
    }

    float slotW = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.width : 135.0f;
    float slotH = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.height : 10.0f;
    float knobW = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.width : 22.0f;
    float knobH = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.height : 29.0f;
    float travelW = slotW - knobW;

    // 1. Music Volume Section
    Rectangle musicLabelRect = { dialogX + 45.0f, dialogY + 137.0f, 140.0f, 25.0f };
    m_font.DrawTextCentered("Music", musicLabelRect, 0.80f, Color{ 210, 210, 210, 255 });

    float musicSlotX = dialogX + 205.0f;
    float musicSlotY = dialogY + 145.0f;
    if (m_sliderSlot.id != 0) {
        DrawTexture(m_sliderSlot, (int)musicSlotX, (int)musicSlotY, WHITE);
    } else {
        DrawRectangleRec({ musicSlotX, musicSlotY, slotW, slotH }, DARKGRAY);
    }

    float musicVol = AudioManager::GetInstance().GetMusicVolume();
    float musicKnobX = musicSlotX + musicVol * travelW;
    float musicKnobY = musicSlotY + (slotH - knobH) / 2.0f;
    if (m_sliderKnob.id != 0) {
        DrawTexture(m_sliderKnob, (int)musicKnobX, (int)musicKnobY, WHITE);
    } else {
        DrawRectangleRec({ musicKnobX, musicKnobY, knobW, knobH }, GRAY);
    }

    // 2. Sound FX Volume Section
    Rectangle sfxLabelRect = { dialogX + 45.0f, dialogY + 182.0f, 140.0f, 25.0f };
    m_font.DrawTextCentered("Sound FX", sfxLabelRect, 0.80f, Color{ 210, 210, 210, 255 });

    float sfxSlotX = dialogX + 205.0f;
    float sfxSlotY = dialogY + 190.0f;
    if (m_sliderSlot.id != 0) {
        DrawTexture(m_sliderSlot, (int)sfxSlotX, (int)sfxSlotY, WHITE);
    } else {
        DrawRectangleRec({ sfxSlotX, sfxSlotY, slotW, slotH }, DARKGRAY);
    }

    float sfxVol = AudioManager::GetInstance().GetSoundVolume();
    float sfxKnobX = sfxSlotX + sfxVol * travelW;
    float sfxKnobY = sfxSlotY + (slotH - knobH) / 2.0f;
    if (m_sliderKnob.id != 0) {
        DrawTexture(m_sliderKnob, (int)sfxKnobX, (int)sfxKnobY, WHITE);
    } else {
        DrawRectangleRec({ sfxKnobX, sfxKnobY, knobW, knobH }, GRAY);
    }

    // 3. Restart Level button
    Rectangle restartRect = { dialogX + 91.5f, dialogY + 278.0f, 240.0f, 44.0f };
    bool restartHovered = CheckCollisionPointRec(mousePos, restartRect);
    bool restartPressed = restartHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    draw3SliceButton(restartRect, "Restart Level", restartHovered, restartPressed);

    // 4. Main Menu button
    Rectangle mainMenuRect = { dialogX + 91.5f, dialogY + 330.0f, 240.0f, 44.0f };
    bool mainMenuHovered = CheckCollisionPointRec(mousePos, mainMenuRect);
    bool mainMenuPressed = mainMenuHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    draw3SliceButton(mainMenuRect, "Main Menu", mainMenuHovered, mainMenuPressed);

    // 5. BACK TO GAME button
    Rectangle backRect = { dialogX + 31.5f, dialogY + 385.0f, 360.0f, 100.0f };
    bool backHovered = CheckCollisionPointRec(mousePos, backRect);
    bool backPressed = backHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    Texture2D curBackTex = (backHovered && m_backToGameBtnHl.id != 0) ? m_backToGameBtnHl : m_backToGameBtn;
    if (curBackTex.id != 0) {
        DrawTexture(curBackTex, (int)backRect.x, (int)backRect.y, WHITE);

        Rectangle backTextRect = { backRect.x, backRect.y + 14.0f, backRect.width, 72.0f };
        if (backPressed) {
            backTextRect.y += 2.0f;
        }
        Color backTextColor = backHovered ? Color{ 100, 255, 100, 255 } : Color{ 0, 230, 0, 255 };
        m_font.DrawTextCentered("BACK TO GAME", backTextRect, 1.15f, backTextColor);
    } else {
        draw3SliceButton(backRect, "BACK TO GAME", backHovered, backPressed);
    }
}

void InGameMenu::drawMenuButton(bool hovered, bool pressed) {
    draw3SliceButton(GetMenuButtonRect(), "Menu", hovered, pressed);
}

void InGameMenu::draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed) {
    Texture2D leftTex   = pressed ? m_btnLeftDown : m_btnLeft;
    Texture2D midTex    = pressed ? m_btnMiddleDown : m_btnMiddle;
    Texture2D rightTex  = pressed ? m_btnRightDown : m_btnRight;

    if (leftTex.id != 0 && midTex.id != 0 && rightTex.id != 0) {
        float scaleY = (leftTex.height > 0) ? (rect.height / (float)leftTex.height) : 1.0f;
        float leftW = (float)leftTex.width * scaleY;
        float rightW = (float)rightTex.width * scaleY;
        float midX = rect.x + leftW;
        float midW = rect.width - leftW - rightW;

        // 1. Draw left slice (scaled to match rect.height)
        DrawTexturePro(
            leftTex,
            { 0.0f, 0.0f, (float)leftTex.width, (float)leftTex.height },
            { rect.x, rect.y, leftW, rect.height },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        // 2. Draw middle slice (stretched to fill the remaining width)
        if (midW > 0.0f) {
            DrawTexturePro(
                midTex,
                { 0.0f, 0.0f, (float)midTex.width, (float)midTex.height },
                { midX, rect.y, midW, rect.height },
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }

        // 3. Draw right slice (scaled to match rect.height)
        DrawTexturePro(
            rightTex,
            { 0.0f, 0.0f, (float)rightTex.width, (float)rightTex.height },
            { rect.x + rect.width - rightW, rect.y, rightW, rect.height },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(rect, pressed ? DARKGRAY : (hovered ? GRAY : LIGHTGRAY));
        DrawRectangleLinesEx(rect, 2.0f, BLACK);
    }

    Rectangle textRect = rect;
    if (pressed) {
        textRect.y += 2.0f;
    }
    Color textColor = hovered ? Color{ 100, 255, 100, 255 } : Color{ 0, 230, 0, 255 };
    float fontScale = 0.85f * (rect.height / 44.0f);
    m_font.DrawTextCentered(text, textRect, fontScale, textColor);
}
