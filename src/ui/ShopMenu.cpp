#include "ShopMenu.h"
#include "UIHelpers.h"
#include <iostream>

ShopMenu::ShopMenu(Resources& res)
    : m_res(res), m_currentPage(0), m_totalPages(4) {
    // Load store background, car, and button textures
    m_shopBack          = res.GetTexture("STORE_BACKGROUND");
    m_car               = res.GetTexture("STORE_CAR");
    m_mainMenuBtn       = res.GetTexture("STORE_MAINMENUBUTTON");
    m_mainMenuBtnHl     = res.GetTexture("STORE_MAINMENUBUTTONHIGHLIGHT");
    m_mainMenuBtnDown   = res.GetTexture("STORE_MAINMENUBUTTONDOWN");
    m_shopSign          = res.GetTexture("STORE_SIGN");

    // Load PREV / NEXT button textures
    m_prevBtn           = res.GetTexture("STORE_PREVBUTTON");
    m_prevBtnDisabled   = res.GetTexture("STORE_PREVBUTTONDISABLED");
    m_prevBtnHl         = res.GetTexture("STORE_PREVBUTTONHIGHLIGHT");
    m_nextBtn           = res.GetTexture("STORE_NEXTBUTTON");
    m_nextBtnDisabled   = res.GetTexture("STORE_NEXTBUTTONDISABLED");
    m_nextBtnHl         = res.GetTexture("STORE_NEXTBUTTONHIGHLIGHT");

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

    // Grid coordinates for the 8 shelf slots
    struct SlotCoord {
        float x, y, w, h;
    };
    SlotCoord slots[] = {
        // Row 1 (Lower Shelf, y = 310)
        { 372.0f, 310.0f, 50.0f, 70.0f },
        { 446.0f, 310.0f, 50.0f, 70.0f },
        { 520.0f, 310.0f, 50.0f, 70.0f },
        { 594.0f, 310.0f, 50.0f, 70.0f },
        // Row 2 (Upper Shelf, y = 206)
        { 420.0f, 206.0f, 50.0f, 70.0f },
        { 494.0f, 206.0f, 50.0f, 70.0f },
        { 568.0f, 206.0f, 50.0f, 70.0f },
        { 642.0f, 206.0f, 50.0f, 70.0f },
    };

    // Item definitions for 4 pages (8 items per page)
    struct ItemRawDef {
        const char* name;
        const char* key;
    };

    ItemRawDef pagesDefs[4][8] = {
        // Page 1
        {
            { "Gatling Pea",     "GATLINGPEA" },
            { "Twin Sunflower",  "TWINSUNFLOWER" },
            { "Gloom-Shroom",    "GLOOMSHROOM" },
            { "Cattail",         "CATTAIL" },
            { "Winter Melon",    "WINTERMELON" },
            { "Gold Magnet",     "GOLDMAGNET" },
            { "Spikerock",       "SPIKEROCK" },
            { "Cob Cannon",      "COBCANNON" },
        },
        // Page 2
        {
            { "Imitater",        "IMITATER" },
            { "Jalapeno",        "JALAPENO" },
            { "Squash",          "SQUASH" },
            { "Potato Mine",     "POTATOMINE" },
            { "Cherry Bomb",     "CHERRYBOMB" },
            { "Garlic",          "GARLIC" },
            { "Pumpkin",         "PUMPKIN" },
            { "Torchwood",       "TORCHWOOD" },
        },
        // Page 3
        {
            { "Melon-Pult",      "MELONPULT" },
            { "Cabbage-Pult",    "CABBAGEPULT" },
            { "Corn-Pult",       "CORNPULT" },
            { "Coffee Bean",     "COFFEEBEAN" },
            { "Doom-Shroom",     "DOOMSHROOM" },
            { "Ice-Shroom",      "ICESHROOM" },
            { "Hypno-Shroom",    "HYPNOSHROOM" },
            { "Scaredy-Shroom",  "SCAREDYSHROOM" },
        },
        // Page 4
        {
            { "Puff-Shroom",     "PUFFSHROOM" },
            { "Sun-Shroom",      "SUNSHROOM" },
            { "Fume-Shroom",     "FUMESHROOM" },
            { "Magnet-Shroom",   "MAGNETSHROOM" },
            { "Lily Pad",        "LILYPAD" },
            { "Tangle Kelp",     "TANGLEKELP" },
            { "Sea-Shroom",      "SEASHROOM" },
            { "Plantern",        "PLANTERN" },
        }
    };

    m_pages.resize(4);
    for (int p = 0; p < 4; p++) {
        for (int i = 0; i < 8; i++) {
            Texture2D tex = res.GetTexture(pagesDefs[p][i].key);
            m_pages[p].push_back({
                pagesDefs[p][i].name,
                pagesDefs[p][i].key,
                tex,
                { slots[i].x, slots[i].y, slots[i].w, slots[i].h },
                false
            });
        }
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

    // PREV Button Logic (Left tail light at 251, 402)
    float prevW = (m_prevBtn.id != 0) ? (float)m_prevBtn.width : 96.0f;
    float prevH = (m_prevBtn.id != 0) ? (float)m_prevBtn.height : 75.0f;
    Rectangle prevRect = { 251.0f, 402.0f, prevW, prevH };

    if (m_currentPage > 0) {
        if (isButtonHovered(mousePos, prevRect, "STORE_PREVBUTTON")) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_currentPage--;
            }
        }
    }

    // NEXT Button Logic (Right tail light at 600, 402)
    float nextW = (m_nextBtn.id != 0) ? (float)m_nextBtn.width : 136.0f;
    float nextH = (m_nextBtn.id != 0) ? (float)m_nextBtn.height : 83.0f;
    Rectangle nextRect = { 597.0f, 402.0f, nextW, nextH };

    if (m_currentPage < m_totalPages - 1) {
        if (isButtonHovered(mousePos, nextRect, "STORE_NEXTBUTTON")) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_currentPage++;
            }
        }
    }

    // Update pixel-perfect hover checks for current page seed packets
    if (m_currentPage >= 0 && m_currentPage < (int)m_pages.size()) {
        for (auto& item : m_pages[m_currentPage]) {
            item.hovered = isButtonHovered(mousePos, item.bounds, item.textureKey);
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

    // 2. Draw Car (STORE_CAR at 196, 138)
    if (m_car.id != 0) {
        DrawTexture(m_car, 196, 138, WHITE);
    }

    // 3. Draw Store Sign if loaded
    if (m_shopSign.id != 0) {
        DrawTexture(m_shopSign, 250, 0, WHITE);
    }

    // 4. Draw Crazy Dave (left side of 800x600 window in front of fence)
    m_crazyDave.Draw(-40.0f, 50.0f, 1.0f);

    // 5. Draw Shop Seed Packets for current page with pixel-perfect hover highlight
    if (m_currentPage >= 0 && m_currentPage < (int)m_pages.size()) {
        for (const auto& item : m_pages[m_currentPage]) {
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
    }

    // 6. Draw PREV Button (Left tail light at 251, 402)
    float prevW = (m_prevBtn.id != 0) ? (float)m_prevBtn.width : 96.0f;
    float prevH = (m_prevBtn.id != 0) ? (float)m_prevBtn.height : 75.0f;
    Rectangle prevRect = { 251.0f, 402.0f, prevW, prevH };

    if (m_currentPage == 0) {
        // Disabled on first page
        Texture2D tex = (m_prevBtnDisabled.id != 0) ? m_prevBtnDisabled : m_prevBtn;
        DrawTexture(tex, (int)prevRect.x, (int)prevRect.y, WHITE);
    } else {
        // Active on later pages
        bool hovered = isButtonHovered(mousePos, prevRect, "STORE_PREVBUTTON");
        Texture2D tex = hovered ? ((m_prevBtnHl.id != 0) ? m_prevBtnHl : m_prevBtn) : m_prevBtn;
        DrawTexture(tex, (int)prevRect.x, (int)prevRect.y, WHITE);
    }

    // 7. Draw NEXT Button (Right tail light at 600, 402)
    float nextW = (m_nextBtn.id != 0) ? (float)m_nextBtn.width : 136.0f;
    float nextH = (m_nextBtn.id != 0) ? (float)m_nextBtn.height : 83.0f;
    Rectangle nextRect = { 597.0f, 402.0f, nextW, nextH };

    if (m_currentPage == m_totalPages - 1) {
        // Disabled on last page
        Texture2D tex = (m_nextBtnDisabled.id != 0) ? m_nextBtnDisabled : m_nextBtn;
        DrawTexture(tex, (int)nextRect.x, (int)nextRect.y, WHITE);
    } else {
        // Active on earlier pages
        bool hovered = isButtonHovered(mousePos, nextRect, "STORE_NEXTBUTTON");
        Texture2D tex = hovered ? ((m_nextBtnHl.id != 0) ? m_nextBtnHl : m_nextBtn) : m_nextBtn;
        DrawTexture(tex, (int)nextRect.x, (int)nextRect.y, WHITE);
    }

    // 8. Draw Page Indicator Text ("Page X of Y")
    std::string pageText = "Page " + std::to_string(m_currentPage + 1) + " of " + std::to_string(m_totalPages);
    Rectangle pageTextBounds = { 425.0f, 492.0f, 138.0f, 20.0f };
    m_font.DrawTextCentered(pageText.c_str(), pageTextBounds, 0.7f, WHITE);

    // 9. Draw Main Menu license plate button with HouseOfTerror font label (Pixel-perfect hover detection)
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
