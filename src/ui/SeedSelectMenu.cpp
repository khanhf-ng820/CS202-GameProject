#include "SeedSelectMenu.h"
#include "UIHelpers.h"
#include <algorithm>
#include <iostream>

SeedSelectMenu::SeedSelectMenu(Resources& res) {
    m_bankBounds = { 0.0f, 0.0f, 446.0f, 87.0f };
    m_chooserBounds = { 0.0f, 87.0f, 465.0f, 513.0f };
    m_confirmBtnBounds = { 155.0f, 545.0f, 156.0f, 42.0f };

    // Load textures
    m_bankTex = res.GetTexture("SEEDBANK");
    if (m_bankTex.id == 0) m_bankTex = res.GetTexture("SeedBank");

    m_chooserBgTex = res.GetTexture("SEEDCHOOSER_BACKGROUND");
    if (m_chooserBgTex.id == 0) m_chooserBgTex = res.GetTexture("SeedChooser_Background");

    m_confirmBtnTex = res.GetTexture("SEEDCHOOSER_BUTTON");
    if (m_confirmBtnTex.id == 0) m_confirmBtnTex = res.GetTexture("SeedChooser_Button");

    m_confirmBtnGlowTex = res.GetTexture("SEEDCHOOSER_BUTTON_GLOW");
    if (m_confirmBtnGlowTex.id == 0) m_confirmBtnGlowTex = res.GetTexture("SeedChooser_Button_Glow");

    m_confirmBtnDisabledTex = res.GetTexture("SEEDCHOOSER_BUTTON_DISABLED");
    if (m_confirmBtnDisabledTex.id == 0) m_confirmBtnDisabledTex = res.GetTexture("SeedChooser_Button_Disabled");

    // Load fonts
    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_font.Load(fontPng, fontTxt);

    std::string hotPng = res.GetAssetPath("assets/data/HouseofTerror28.png");
    std::string hotTxt = res.GetAssetPath("assets/data/HouseofTerror28.txt");
    m_houseOfTerrorFont.Load(hotPng, hotTxt);

    std::string sunPng = res.GetAssetPath("assets/data/_ContinuumBold14.png");
    std::string sunTxt = res.GetAssetPath("assets/data/ContinuumBold14.txt");
    m_sunFont.Load(sunPng, sunTxt);

    std::string pricePng = res.GetAssetPath("assets/data/_Pico129.png");
    std::string priceTxt = res.GetAssetPath("assets/data/Pico129.txt");
    m_priceFont.Load(pricePng, priceTxt);

    initCatalog(res);
}

void SeedSelectMenu::initCatalog(Resources& res) {
    struct SeedCatalogEntry {
        const char* type;
        int cost;
        const char* texKey;
    };

    SeedCatalogEntry entries[] = {
        { "SunFlower",   50,  "SUNFLOWER" },
        { "PeaShooter",  100, "PEASHOOTER" },
        { "Wallnut",     50,  "WALLNUT" },
        { "SnowPea",     175, "SNOWPEA" },
        { "Repeater",    200, "REPEATER" },
        { "CherryBomb",  150, "CHERRYBOMB" },
        { "Jalapeno",    125, "JALAPENO" },
        { "GatlingPea",  250, "GATLINGPEA" },
        { "Cornpult",    100, "CORNPULT" },
        { "Melonpult",   300, "MELONPULT" },
        { "Torchwood",   175, "TORCHWOOD" },
        { "Chomper",     150, "CHOMPER" }
    };

    float startX = 22.0f;
    float startY = 135.0f;
    float spacingX = 54.0f;
    float spacingY = 74.0f;
    float cardW = 50.0f;
    float cardH = 70.0f;

    int cols = 4;
    int index = 0;

    for (const auto& entry : entries) {
        int r = index / cols;
        int c = index % cols;

        float x = startX + c * spacingX;
        float y = startY + r * spacingY;

        m_availableCards.push_back({
            entry.type,
            entry.cost,
            entry.texKey,
            { x, y, cardW, cardH },
            false
        });

        index++;
    }
}

bool SeedSelectMenu::update(float dt, Vector2 mousePos, bool mouseClicked) {
    if (!mouseClicked) return false;

    // 1. Check top SeedBank slots (unselect plant if clicked in top bar)
    for (size_t i = 0; i < m_chosenPlants.size(); ++i) {
        Rectangle slotRect = { 80.0f + i * 53.0f, 8.0f, 50.0f, 70.0f };
        if (CheckCollisionPointRec(mousePos, slotRect)) {
            std::string typeToRemove = m_chosenPlants[i];
            m_chosenPlants.erase(m_chosenPlants.begin() + i);

            // Mark card as unselected in chooser grid
            for (auto& card : m_availableCards) {
                if (card.plantType == typeToRemove) {
                    card.isChosen = false;
                    break;
                }
            }
            return false;
        }
    }

    // 2. Check chooser panel grid cards (select or unselect)
    for (auto& card : m_availableCards) {
        if (CheckCollisionPointRec(mousePos, card.bounds)) {
            if (card.isChosen) {
                // Unselect
                card.isChosen = false;
                auto it = std::find(m_chosenPlants.begin(), m_chosenPlants.end(), card.plantType);
                if (it != m_chosenPlants.end()) {
                    m_chosenPlants.erase(it);
                }
            } else {
                // Select (up to max 7 capacity)
                if (m_chosenPlants.size() < 7) {
                    card.isChosen = true;
                    m_chosenPlants.push_back(card.plantType);
                }
            }
            return false;
        }
    }

    // 3. Check confirm button ("LET'S ROCK!")
    if (!m_chosenPlants.empty() && CheckCollisionPointRec(mousePos, m_confirmBtnBounds)) {
        return true; // Confirmed!
    }

    return false;
}

void SeedSelectMenu::draw(Resources& res, Vector2 mousePos) const {
    // 1. Draw Top SeedBank Bar at (0, 0)
    if (m_bankTex.id != 0) {
        DrawTexture(m_bankTex, 0, 0, WHITE);
    } else {
        DrawRectangleRec(m_bankBounds, DARKGRAY);
    }

    // Draw Sun Count on top SeedBank
    std::string sunText = "40000";
    m_sunFont.DrawText(sunText.c_str(), 18.0f, 58.0f, 0.9f, BLACK);

    // Draw chosen seed packets in top SeedBank slots
    for (size_t i = 0; i < m_chosenPlants.size(); ++i) {
        float slotX = 80.0f + i * 53.0f;
        float slotY = 8.0f;
        Rectangle slotRect = { slotX, slotY, 50.0f, 70.0f };

        std::string pType = m_chosenPlants[i];
        Texture2D tex = res.GetTexture(pType);
        if (tex.id != 0) {
            DrawTexturePro(
                tex,
                { 0.0f, 0.0f, (float)tex.width, (float)tex.height },
                slotRect,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(slotRect, GREEN);
            m_font.DrawTextCentered(pType.c_str(), slotRect, 0.4f, WHITE);
        }
    }

    // 2. Draw Chooser Background Panel at (0, 87)
    if (m_chooserBgTex.id != 0) {
        DrawTexture(m_chooserBgTex, 0, 87, WHITE);
    } else {
        DrawRectangleRec(m_chooserBounds, Color{ 40, 30, 20, 240 });
    }

    // Draw header text
    m_font.DrawText("Choose your seeds", 130.0f, 98.0f, 0.65f, GOLD);

    // 3. Draw available cards grid
    for (const auto& card : m_availableCards) {
        Texture2D cardTex = res.GetTexture(card.plantType);

        if (card.isChosen) {
            // Draw dimmed / grayed out silhouette
            if (cardTex.id != 0) {
                DrawTexturePro(
                    cardTex,
                    { 0.0f, 0.0f, (float)cardTex.width, (float)cardTex.height },
                    card.bounds,
                    { 0.0f, 0.0f },
                    0.0f,
                    Color{ 80, 80, 80, 160 }
                );
            } else {
                DrawRectangleRec(card.bounds, DARKGRAY);
            }
        } else {
            // Draw active card
            if (cardTex.id != 0) {
                DrawTexturePro(
                    cardTex,
                    { 0.0f, 0.0f, (float)cardTex.width, (float)cardTex.height },
                    card.bounds,
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            } else {
                DrawRectangleRec(card.bounds, GREEN);
                m_font.DrawTextCentered(card.plantType.c_str(), card.bounds, 0.4f, WHITE);
            }

            // Draw card cost
            std::string costStr = std::to_string(card.cost);
            int costWidth = m_priceFont.MeasureText(costStr.c_str(), 0.6f);
            float textX = card.bounds.x + (card.bounds.width - (float)costWidth) / 2.0f;
            float textY = card.bounds.y + card.bounds.height - 18.0f;
            m_priceFont.DrawText(costStr.c_str(), textX, textY, 0.6f, BLACK);
        }
    }

    // 4. Draw Confirm Button ("LET'S ROCK!")
    bool isHovered = CheckCollisionPointRec(mousePos, m_confirmBtnBounds);
    bool isEnabled = !m_chosenPlants.empty();

    Texture2D btnTex = m_confirmBtnTex;
    if (!isEnabled) {
        if (m_confirmBtnDisabledTex.id != 0) btnTex = m_confirmBtnDisabledTex;
    } else if (isHovered && m_confirmBtnGlowTex.id != 0) {
        btnTex = m_confirmBtnGlowTex;
    }

    if (btnTex.id != 0) {
        DrawTexturePro(
            btnTex,
            { 0.0f, 0.0f, (float)btnTex.width, (float)btnTex.height },
            m_confirmBtnBounds,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(m_confirmBtnBounds, isEnabled ? (isHovered ? GOLD : ORANGE) : GRAY);
    }

    // Centered "LET'S ROCK!" label on confirm button
    Color labelColor = isEnabled ? Color{ 220, 200, 80, 255 } : Color{ 120, 120, 120, 255 };
    m_houseOfTerrorFont.DrawTextCentered("LET'S ROCK!", m_confirmBtnBounds, 0.65f, labelColor);
}
