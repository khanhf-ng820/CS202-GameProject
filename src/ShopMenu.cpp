#include "ShopMenu.h"
#include "UIHelpers.h"
#include <iostream>

ShopMenu::ShopMenu(Resources& res)
    : m_res(res) {
    // Load store background, car, and button textures
    m_shopBack          = res.GetTexture("STORE_BACKGROUND");
    m_car               = res.GetTexture("STORE_CAR");
    m_mainMenuBtn       = res.GetTexture("STORE_MAINMENUBUTTON");
    m_mainMenuBtnHl     = res.GetTexture("STORE_MAINMENUBUTTONHIGHLIGHT");
    m_mainMenuBtnDown   = res.GetTexture("STORE_MAINMENUBUTTONDOWN");
    m_shopSign          = res.GetTexture("STORE_SIGN");

    // Load Crazy Dave reanim
    std::string davePath = res.GetAssetPath("assets/reanim/CrazyDave.reanim");
    ReanimDefinition daveDef = res.LoadReanim(davePath);
    m_crazyDave.SetResources(daveDef, res);
    m_crazyDave.SetAnimation("anim_idle");

    // Load fonts
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);

    std::string hotPng = res.GetAssetPath("assets/data/HouseofTerror28.png");
    std::string hotTxt = res.GetAssetPath("assets/data/HouseofTerror28.txt");
    m_houseOfTerrorFont.Load(hotPng, hotTxt);

    // Initialize 8 seed packet items for the shop shelves according to exact user coordinates
    struct SeedDef {
        const char* name;
        const char* key;
        float x;
        float y;
        float w;
        float h;
    };

    SeedDef seedDefs[] = {
        // Row 1 (Lower Shelf, y = 310)
        { "Gatling Pea",     "GATLINGPEA",     372.0f, 310.0f, 50.0f, 70.0f },
        { "Twin Sunflower",  "TWINSUNFLOWER",  446.0f, 310.0f, 50.0f, 70.0f },
        { "Gloom-Shroom",    "GLOOMSHROOM",    520.0f, 310.0f, 50.0f, 70.0f },
        { "Cattail",         "CATTAIL",        594.0f, 310.0f, 50.0f, 70.0f },
        // Row 2 (Upper Shelf, y = 206)
        { "Winter Melon",    "WINTERMELON",    420.0f, 206.0f, 50.0f, 70.0f },
        { "Gold Magnet",     "GOLDMAGNET",     494.0f, 206.0f, 50.0f, 70.0f },
        { "Spikerock",       "SPIKEROCK",      568.0f, 206.0f, 50.0f, 70.0f },
        { "Cob Cannon",      "COBCANNON",      642.0f, 206.0f, 50.0f, 70.0f },
    };

    for (const auto& def : seedDefs) {
        Texture2D tex = res.GetTexture(def.key);
        m_shopItems.push_back({
            def.name,
            def.key,
            tex,
            { def.x, def.y, def.w, def.h },
            false
        });
    }
}

bool ShopMenu::isButtonHovered(Vector2 mousePos, Rectangle bounds, const std::string& texName) {
    if (!CheckCollisionPointRec(mousePos, bounds)) return false;
    Texture2D tex = m_res.GetTexture(texName);
    float scaleX = (tex.width > 0 && bounds.width > 0) ? (float)tex.width / bounds.width : 1.0f;
    float scaleY = (tex.height > 0 && bounds.height > 0) ? (float)tex.height / bounds.height : 1.0f;
    int localX = (int)((mousePos.x - bounds.x) * scaleX);
    int localY = (int)((mousePos.y - bounds.y) * scaleY);
    return !m_res.IsPixelTransparent(texName, localX, localY);
}

void ShopMenu::update(float dt, bool& showShop) {
    Vector2 mousePos = GetVirtualMousePosition();

    // Update Crazy Dave animation
    m_crazyDave.Update(dt);

    // Position of Main Menu license plate button on Crazy Dave's trunk in original 800x600 canvas
    float btnW = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.width : 138.0f;
    float btnH = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.height : 80.0f;
    
    Rectangle btnRect = { 425.0f, 514.0f, btnW, btnH };

    if (isButtonHovered(mousePos, btnRect, "STORE_MAINMENUBUTTON")) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showShop = false;
        }
    }

    // Update pixel-perfect hover checks for seed packets
    for (auto& item : m_shopItems) {
        item.hovered = isButtonHovered(mousePos, item.bounds, item.textureKey);
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

    // 2. Draw Car (STORE_CAR at 196, 138)
    if (m_car.id != 0) {
        DrawTexture(m_car, 196, 138, WHITE);
    }

    // 3. Draw Store Sign if loaded
    if (m_shopSign.id != 0) {
        DrawTexture(m_shopSign, 250, 0, WHITE);
    }

    // 4. Draw Crazy Dave (left side of 800x600 window in front of fence)
    m_crazyDave.Draw(0.0f, 50.0f, 1.0f);

    // 5. Draw Shop Seed Packets with pixel-perfect hover highlight
    for (const auto& item : m_shopItems) {
        if (item.texture.id != 0) {
            DrawTexturePro(
                item.texture,
                { 0.0f, 0.0f, (float)item.texture.width, (float)item.texture.height },
                item.bounds,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
            if (item.hovered) {
                // Pixel-perfect hover highlight (yellow border glow)
                DrawRectangleLinesEx(item.bounds, 2.0f, YELLOW);
            }
        } else {
            Color boxColor = item.hovered ? ColorAlpha(YELLOW, 0.8f) : ColorAlpha(GRAY, 0.8f);
            DrawRectangleRec(item.bounds, boxColor);
            m_font.DrawTextCentered(item.name.c_str(), item.bounds, 0.5f, WHITE);
        }
    }

    // 6. Draw Main Menu license plate button with HouseOfTerror font label (Pixel-perfect hover detection)
    float btnW = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.width : 138.0f;
    float btnH = (m_mainMenuBtn.id != 0) ? (float)m_mainMenuBtn.height : 80.0f;
    Rectangle btnRect = { 425.0f, 514.0f, btnW, btnH };

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
        m_houseOfTerrorFont.DrawTextCentered("MAIN MENU", { btnRect.x - 8.f, btnRect.y - 4.0f, btnRect.width, btnRect.height }, 0.75f, Color{ 102, 152, 235, 255 });
    } else {
        DrawRectangleRec(btnRect, BROWN);
        m_houseOfTerrorFont.DrawTextCentered("MAIN MENU", { btnRect.x - 8.f, btnRect.y - 4.0f, btnRect.width, btnRect.height }, 0.75f, Color{ 102, 152, 235, 255 });
    }
}
