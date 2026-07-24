#include "ShopMenu.h"
#include "UIHelpers.h"
#include <iostream>

ShopMenu::ShopMenu(Resources& res)
    : m_res(res) {
    // Load store background and button textures
    m_shopBack          = res.GetTexture("STORE_BACKGROUND");
    m_mainMenuBtn       = res.GetTexture("STORE_MAINMENUBUTTON");
    m_mainMenuBtnHl     = res.GetTexture("STORE_MAINMENUBUTTONHIGHLIGHT");
    m_mainMenuBtnDown   = res.GetTexture("STORE_MAINMENUBUTTONDOWN");
    m_shopSign          = res.GetTexture("STORE_SIGN");

    // Load font
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);
}

bool ShopMenu::isButtonHovered(Vector2 mousePos, Rectangle bounds, const std::string& texName) {
    if (!CheckCollisionPointRec(mousePos, bounds)) return false;
    int localX = (int)(mousePos.x - bounds.x);
    int localY = (int)(mousePos.y - bounds.y);
    return !m_res.IsPixelTransparent(texName, localX, localY);
}

void ShopMenu::update(float dt, bool& showShop) {
    Vector2 mousePos = GetVirtualMousePosition();

    // Position of Main Menu license plate button on Crazy Dave's trunk
    float btnW = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.width : 160.0f;
    float btnH = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.height : 60.0f;
    
    // License plate position in original PvZ store background (800x600 canvas)
    Rectangle btnRect = { 640.0f, 510.0f, btnW, btnH };

    if (isButtonHovered(mousePos, btnRect, "STORE_MAINMENUBUTTON")) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showShop = false;
        }
    }
}

void ShopMenu::draw() {
    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Draw Shop background (800x600 canvas)
    if (m_shopBack.id != 0) {
        DrawTexturePro(
            m_shopBack,
            { 0.0f, 0.0f, (float)m_shopBack.width, (float)m_shopBack.height },
            { 0.0f, 0.0f, 800.0f, 600.0f },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        ClearBackground(DARKGRAY);
    }

    // 2. Draw Store Sign if loaded
    if (m_shopSign.id != 0) {
        DrawTexture(m_shopSign, 250, 0, WHITE);
    }

    // 3. Draw Main Menu license plate button (Pixel-perfect hover detection)
    float btnW = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.width : 160.0f;
    float btnH = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.height : 60.0f;
    Rectangle btnRect = { 640.0f, 510.0f, btnW, btnH };

    if (m_mainMenuBtn.id != 0) {
        bool hovered = isButtonHovered(mousePos, btnRect, "STORE_MAINMENUBUTTON");
        bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        Texture2D tex = m_mainMenuBtn;
        if (pressed && m_mainMenuBtnDown.id != 0) {
            tex = m_mainMenuBtnDown;
        } else if (hovered && m_mainMenuBtnHl.id != 0) {
            tex = m_mainMenuBtnHl;
        }

        DrawTexture(tex, (int)btnRect.x, (int)btnRect.y, WHITE);
    } else {
        DrawRectangleRec(btnRect, BROWN);
        m_font.DrawTextCentered("Main Menu", btnRect, 0.8f, WHITE);
    }
}
