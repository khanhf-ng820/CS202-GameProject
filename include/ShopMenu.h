#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"
#include <string>

class ShopMenu {
public:
    ShopMenu(Resources& res);
    ~ShopMenu() = default;

    void update(float dt, bool& showShop);
    void draw();

private:
    bool isButtonHovered(Vector2 mousePos, Rectangle bounds, const std::string& texName);

    Resources& m_res;
    BitmapFont m_font;

    Texture2D m_shopBack;
    Texture2D m_mainMenuBtn;
    Texture2D m_mainMenuBtnHl;
    Texture2D m_mainMenuBtnDown;
    Texture2D m_shopSign;
};
