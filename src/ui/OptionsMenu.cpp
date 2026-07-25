#include "OptionsMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <iostream>
#include <algorithm>

OptionsMenu::OptionsMenu(Resources& res)
    : m_res(res), m_selectedResolutionIndex(0), m_activeResolutionIndex(0) {
    
    // Load authentic PvZ bitmap font
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);

    // Retrieve options menu panel and button textures
    m_menuBack          = res.GetTexture("OPTIONS_MENUBACK");
    if (m_menuBack.id == 0) {
        m_menuBack = res.GetTexture("OPTIONS_MENUBACK_");
    }
    m_checkboxUnchecked = res.GetTexture("OPTIONS_CHECKBOX0");
    m_checkboxChecked   = res.GetTexture("OPTIONS_CHECKBOX1");
    m_backToGameBtn     = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON0");
    m_backToGameBtnHl   = res.GetTexture("OPTIONS_BACKTOGAMEBUTTON2");

    // Slider textures
    m_sliderSlot        = res.GetTexture("OPTIONS_SLIDERSLOT");
    m_sliderKnob        = res.GetTexture("OPTIONS_SLIDERKNOB2");

    // 3-slice buttons for Apply Changes
    m_btnLeft           = res.GetTexture("BUTTON_LEFT");
    m_btnMiddle         = res.GetTexture("BUTTON_MIDDLE");
    m_btnRight          = res.GetTexture("BUTTON_RIGHT");
    m_btnLeftDown       = res.GetTexture("BUTTON_DOWN_LEFT");
    m_btnMiddleDown     = res.GetTexture("BUTTON_DOWN_MIDDLE");
    m_btnRightDown      = res.GetTexture("BUTTON_DOWN_RIGHT");

    // Initialize resolution indexes based on current window size
    int currentW = GetScreenWidth();
    int currentH = GetScreenHeight();
    for (int i = 0; i < NUM_RESOLUTIONS; ++i) {
        if (RESOLUTION_PRESETS[i].width == currentW && RESOLUTION_PRESETS[i].height == currentH) {
            m_activeResolutionIndex = i;
            m_selectedResolutionIndex = i;
            break;
        }
    }
}

void OptionsMenu::update(float dt, bool& showOptions, int& currentWidth, int& currentHeight) {
    Vector2 mousePos = GetVirtualMousePosition();

    // Dialog layout coordinates in virtual space (800x600)
    float dialogX = (800.0f - 423.0f) / 2.0f;
    float dialogY = (600.0f - 498.0f) / 2.0f;

    // 1. Handle Music Volume Slider dragging
    float slotX = dialogX + 205.0f;
    float slotY = dialogY + 122.0f;
    float slotW = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.width : 135.0f;
    float knobW = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.width : 22.0f;
    Rectangle sliderHitbox = { slotX - 10.0f, slotY - 15.0f, slotW + 20.0f, 40.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, sliderHitbox)) {
        m_isDraggingSlider = true;
    }
    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        m_isDraggingSlider = false;
    }

    if (m_isDraggingSlider) {
        float travelW = slotW - knobW;
        float clampedX = std::clamp(mousePos.x - slotX - knobW / 2.0f, 0.0f, travelW);
        float newVol = (travelW > 0.0f) ? (clampedX / travelW) : 1.0f;
        AudioManager::GetInstance().SetMusicVolume(newVol);
    }

    // Checkbox scale and dimensions
    float cbScale = 0.75f;
    float cbH = 39.0f * cbScale;

    // 2. Handle clicking on resolutions (checkboxes / labels)
    for (int i = 0; i < NUM_RESOLUTIONS; ++i) {
        float itemY = dialogY + 160.0f + i * 42.0f;
        // Collision rectangle spans from checkbox to label text
        Rectangle hitRect = { dialogX + 60.0f, itemY, 300.0f, cbH };
        if (CheckCollisionPointRec(mousePos, hitRect)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_selectedResolutionIndex = i;
            }
        }
    }

    // 3. Handle "Apply Changes" button
    float applyW = 170.0f;
    float applyH = 46.0f;
    float applyX = dialogX + 31.5f;
    float applyY = dialogY + 420.0f;
    Rectangle applyRect = { applyX, applyY, applyW, applyH };
    
    bool applyHovered = CheckCollisionPointRec(mousePos, applyRect);
    if (applyHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_activeResolutionIndex = m_selectedResolutionIndex;
        currentWidth = RESOLUTION_PRESETS[m_activeResolutionIndex].width;
        currentHeight = RESOLUTION_PRESETS[m_activeResolutionIndex].height;
        SetWindowSize(currentWidth, currentHeight);
    }

    // 4. Handle "Back" button
    float backW = 170.0f;
    float backH = 46.0f;
    float backX = dialogX + 31.5f + 170.0f + 20.0f; // 221.5f
    float backY = dialogY + 420.0f;
    Rectangle backRect = { backX, backY, backW, backH };

    bool backHovered = CheckCollisionPointRec(mousePos, backRect);
    if (backHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Revert unapplied resolution selection
        m_selectedResolutionIndex = m_activeResolutionIndex;
        showOptions = false;
    }
}

void OptionsMenu::draw() {
    float dialogX = (800.0f - 423.0f) / 2.0f;
    float dialogY = (600.0f - 498.0f) / 2.0f;

    // Draw dark transparent overlay over screen to block clicks and dim background
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.5f));

    // Draw main background board
    if (m_menuBack.id != 0) {
        DrawTexture(m_menuBack, (int)dialogX, (int)dialogY, WHITE);
    } else {
        DrawRectangleRec({ dialogX, dialogY, 423, 498 }, ColorAlpha(DARKGRAY, 0.95f));
        DrawRectangleLinesEx({ dialogX, dialogY, 423, 498 }, 4.0f, BROWN);
    }

    // Draw OPTIONS title
    m_font.DrawTextCentered("OPTIONS", { dialogX, dialogY + 55.0f, 423.0f, 35.0f }, 1.3f, Color{ 220, 180, 80, 255 });

    // Draw Music Volume Section
    Rectangle labelRect = { dialogX + 45.0f, dialogY + 115.0f, 140.0f, 25.0f };
    m_font.DrawTextCentered("Music Volume", labelRect, 0.70f, Color{ 210, 210, 210, 255 });

    float slotX = dialogX + 205.0f;
    float slotY = dialogY + 122.0f;
    float slotW = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.width : 135.0f;
    float slotH = (m_sliderSlot.id != 0) ? (float)m_sliderSlot.height : 10.0f;
    float knobW = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.width : 22.0f;
    float knobH = (m_sliderKnob.id != 0) ? (float)m_sliderKnob.height : 29.0f;

    if (m_sliderSlot.id != 0) {
        DrawTexture(m_sliderSlot, (int)slotX, (int)slotY, WHITE);
    } else {
        DrawRectangleRec({ slotX, slotY, slotW, slotH }, DARKGRAY);
    }

    float currentVol = AudioManager::GetInstance().GetMusicVolume();
    float travelW = slotW - knobW;
    float knobX = slotX + currentVol * travelW;
    float knobY = slotY + (slotH - knobH) / 2.0f;

    if (m_sliderKnob.id != 0) {
        DrawTexture(m_sliderKnob, (int)knobX, (int)knobY, WHITE);
    } else {
        DrawRectangleRec({ knobX, knobY, knobW, knobH }, GRAY);
    }

    // Draw resolution checklist
    Vector2 mousePos = GetVirtualMousePosition();
    float cbScale = 0.75f;
    float cbW = 42.0f * cbScale;
    float cbH = 39.0f * cbScale;

    for (int i = 0; i < NUM_RESOLUTIONS; ++i) {
        float itemY = dialogY + 160.0f + i * 42.0f;
        Rectangle checkboxRect = { dialogX + 60.0f, itemY, cbW, cbH };
        bool isSelected = (m_selectedResolutionIndex == i);

        // Draw Checkbox texture
        Texture2D cbTex = isSelected ? m_checkboxChecked : m_checkboxUnchecked;
        if (cbTex.id != 0) {
            DrawTexturePro(
                cbTex,
                { 0.0f, 0.0f, (float)cbTex.width, (float)cbTex.height },
                checkboxRect,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(checkboxRect, GRAY);
            if (isSelected) {
                DrawRectangleRec({ checkboxRect.x + 8.0f, checkboxRect.y + 8.0f, 17.6f, 15.2f }, GREEN);
            }
        }

        // Draw Resolution label (highlight green on hover)
        Rectangle textRect = { dialogX + 105.0f, itemY, 250.0f, cbH };
        bool itemHovered = CheckCollisionPointRec(mousePos, { dialogX + 60.0f, itemY, 300.0f, cbH });
        Color labelColor = itemHovered ? GREEN : Color{ 210, 210, 210, 255 };
        m_font.DrawTextCentered(RESOLUTION_PRESETS[i].label, textRect, 0.64f, labelColor);
    }

    // Draw Apply Changes Button
    float applyW = 170.0f;
    float applyH = 46.0f;
    float applyX = dialogX + 31.5f;
    float applyY = dialogY + 420.0f;
    Rectangle applyRect = { applyX, applyY, applyW, applyH };
    bool applyHovered = CheckCollisionPointRec(mousePos, applyRect);
    bool applyPressed = applyHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    draw3SliceButton(applyRect, "APPLY", applyHovered, applyPressed);

    // Draw Back Button
    float backW = 170.0f;
    float backH = 46.0f;
    float backX = dialogX + 31.5f + 170.0f + 20.0f; // 221.5f
    float backY = dialogY + 420.0f;
    Rectangle backRect = { backX, backY, backW, backH };
    bool backHovered = CheckCollisionPointRec(mousePos, backRect);
    bool backPressed = backHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    draw3SliceButton(backRect, "BACK", backHovered, backPressed);
}

void OptionsMenu::draw3SliceButton(Rectangle rect, const char* text, bool hovered, bool pressed) {
    Texture2D leftTex   = pressed ? m_btnLeftDown : m_btnLeft;
    Texture2D midTex    = pressed ? m_btnMiddleDown : m_btnMiddle;
    Texture2D rightTex  = pressed ? m_btnRightDown : m_btnRight;

    if (leftTex.id != 0 && midTex.id != 0 && rightTex.id != 0) {
        // Draw left side piece (fixed 36px width)
        DrawTexture(leftTex, (int)rect.x, (int)rect.y, WHITE);

        // Draw middle piece (stretched to fill the remaining width)
        float midX = rect.x + 36.0f;
        float midW = rect.width - 36.0f - 35.0f;
        DrawTexturePro(
            midTex,
            { 0.0f, 0.0f, (float)midTex.width, (float)midTex.height },
            { midX, rect.y, midW, rect.height },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        // Draw right side piece (fixed 35px width)
        DrawTexture(rightTex, (int)(rect.x + rect.width - 35.0f), (int)rect.y, WHITE);
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
    m_font.DrawTextCentered(text, textRect, 0.85f, textColor);
}
