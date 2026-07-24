#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "BitmapFont.h"
#include <string>
#include <vector>

struct ShopSeedItem {
    std::string name;
    std::string textureKey;
    Texture2D texture;
    Rectangle bounds;
    bool hovered;
};

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

    Reanimation m_crazyDave;
    Texture2D m_shopBack;
    Texture2D m_car;
    Texture2D m_mainMenuBtn;
    Texture2D m_mainMenuBtnHl;
    Texture2D m_mainMenuBtnDown;
    Texture2D m_shopSign;

    std::vector<ShopSeedItem> m_shopItems;
};
