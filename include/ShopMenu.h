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
    BitmapFont m_houseOfTerrorFont;

    Reanimation m_crazyDave;
    Texture2D m_shopBack;
    Texture2D m_car;
    Texture2D m_mainMenuBtn;
    Texture2D m_mainMenuBtnHl;
    Texture2D m_mainMenuBtnDown;
    Texture2D m_shopSign;

    Texture2D m_prevBtn;
    Texture2D m_prevBtnDisabled;
    Texture2D m_prevBtnHl;
    Texture2D m_nextBtn;
    Texture2D m_nextBtnDisabled;
    Texture2D m_nextBtnHl;

    int m_currentPage;
    int m_totalPages;

    std::vector<std::vector<ShopSeedItem>> m_pages;
};
