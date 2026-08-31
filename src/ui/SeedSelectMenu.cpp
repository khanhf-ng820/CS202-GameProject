#include "SeedSelectMenu.h"
#include "UIHelpers.h"
#include "ProfileManager.h"
#include "AudioManager.h"
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

    m_silhouetteTex = res.GetTexture("SEEDPACKETSILHOUETTE");
    if (m_silhouetteTex.id == 0) m_silhouetteTex = res.GetTexture("SeedPacketSilhouette");

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
    m_availableCards.clear();
    m_chosenPlants.clear();
    m_plantCosts.clear();

    struct SeedCatalogEntry {
        const char* type;
        int cost;
        const char* texKey;
    };

    SeedCatalogEntry entries[] = {
        { "SunFlower",   50,  "SUNFLOWER" },
        { "PeaShooter",  100, "PEASHOOTER" },
        { "Wallnut",     50,  "WALLNUT" },
        { "PotatoMine",  25,  "POTATOMINE" },
        { "SnowPea",     175, "SNOWPEA" },
        { "Repeater",    200, "REPEATER" },
        { "CherryBomb",  150, "CHERRYBOMB" },
        { "Jalapeno",    125, "JALAPENO" },
        { "Squash",      50,  "SQUASH" },
        { "GatlingPea",  250, "GATLINGPEA" },
        { "Cornpult",    100, "CORNPULT" },
        { "Cabbagepult", 100, "CABBAGEPULT" },
        { "Melonpult",   300, "MELONPULT" },
        { "Torchwood",   175, "TORCHWOOD" },
        { "Chomper",     150, "CHOMPER" },
        { "IceShroom",   75,  "ICESHROOM" },
        { "Gravebuster", 75,  "GRAVEBUSTER" },
        { "Garlic",      50,  "GARLIC" },
        { "Caltrop",     100, "CALTROP" },
        { "SpikeRock",   125, "SPIKEROCK" },
        { "Plantern",    25,  "PLANTERN" },
        { "TwinSunflower",125,"TWINSUNFLOWER" }
    };

    float startX = 22.0f;
    float startY = 135.0f;
    float spacingX = 53.0f;
    float spacingY = 74.0f;
    float cardW = 50.0f;
    float cardH = 70.0f;

    int cols = 8;
    int index = 0;

    for (const auto& entry : entries) {
        // Only show plants currently unlocked in player's profile
        if (!ProfileManager::GetInstance().IsPlantUnlocked(entry.type)) {
            continue;
        }

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

        m_plantCosts[entry.type] = entry.cost;

        index++;
    }
}

bool SeedSelectMenu::update(float dt, Vector2 mousePos, bool mouseClicked) {
    if (!mouseClicked) return false;

    Resources& res = Resources::GetInstance();

    // 1. Check top SeedBank slots (unselect plant if clicked in top bar)
    for (size_t i = 0; i < m_chosenPlants.size(); ++i) {
        Rectangle slotRect = { 79.0f + i * 51.0f, 8.0f, 50.0f, 70.0f };
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
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/tap.ogg"));
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
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/tap.ogg"));
            } else {
                // Select (up to max 7 capacity)
                if (m_chosenPlants.size() < 7) {
                    card.isChosen = true;
                    m_chosenPlants.push_back(card.plantType);
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/seedlift.ogg"));
                } else {
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
                }
            }
            return false;
        }
    }

    // 3. Check confirm button ("LET'S ROCK!")
    if (CheckCollisionPointRec(mousePos, m_confirmBtnBounds)) {
        if (!m_chosenPlants.empty()) {
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/pause.ogg"));
            return true; // Confirmed!
        } else {
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
        }
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

    // Draw Sun Count on top SeedBank (matching SeedBank::draw positioning)
    std::string sunText = "40000";
    m_sunFont.DrawTextCentered(sunText.c_str(), { m_bankBounds.x + 4.0f, m_bankBounds.y + 60.0f, 66.0f, 22.0f }, 0.9f, BLACK);

    auto drawSunCostLabel = [&](int cost, Rectangle bounds, Color tint) {
        std::string costStr = std::to_string(cost);
        int textWidth = m_priceFont.MeasureText(costStr.c_str(), 1.0f);
        float textX = bounds.x + (bounds.width - (float)textWidth) / 2.0f;
        float textY = bounds.y + bounds.height - 16.0f;
        m_priceFont.DrawText(costStr.c_str(), textX, textY, 1.0f, tint);
    };

    // Draw 7 seed packet silhouettes at all 7 slot positions in top SeedBank
    for (size_t i = 0; i < 7; ++i) {
        float slotX = 79.0f + i * 51.0f;
        float slotY = 8.0f;
        Rectangle slotRect = { slotX, slotY, 50.0f, 70.0f };

        if (m_silhouetteTex.id != 0) {
            DrawTexturePro(
                m_silhouetteTex,
                { 0.0f, 0.0f, (float)m_silhouetteTex.width, (float)m_silhouetteTex.height },
                slotRect,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(slotRect, ColorAlpha(BLACK, 0.35f));
        }
    }

    // Draw chosen seed packets on top of silhouettes in top SeedBank slots
    for (size_t i = 0; i < m_chosenPlants.size(); ++i) {
        float slotX = 79.0f + i * 51.0f;
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

        // Draw sun cost label on top SeedBank packet (matching SeedPacket::draw)
        auto it = m_plantCosts.find(pType);
        int cost = (it != m_plantCosts.end()) ? it->second : 100;
        drawSunCostLabel(cost, slotRect, BLACK);
    }

    // 2. Draw Chooser Background Panel at (0, 87)
    if (m_chooserBgTex.id != 0) {
        DrawTexture(m_chooserBgTex, 0, 87, WHITE);
    } else {
        DrawRectangleRec(m_chooserBounds, Color{ 40, 30, 20, 240 });
    }

    // Draw header text centered horizontally & vertically across top bar
    m_font.DrawTextCentered("CHOOSE YOUR PLANTS!", { 0.0f, 87.0f, 465.0f, 33.0f }, 0.65f, GOLD);

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

            // Draw sun cost label on selected/dimmed grid card
            drawSunCostLabel(card.cost, card.bounds, Color{ 60, 60, 60, 200 });
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

            // Draw sun cost label on unselected grid card
            drawSunCostLabel(card.cost, card.bounds, BLACK);
        }
    }

    // 4. Draw Confirm Button ("LET'S ROCK!")
    bool isHovered = CheckCollisionPointRec(mousePos, m_confirmBtnBounds);
    bool isEnabled = !m_chosenPlants.empty();

    if (!isEnabled) {
        if (m_confirmBtnDisabledTex.id != 0) {
            DrawTexturePro(
                m_confirmBtnDisabledTex,
                { 0.0f, 0.0f, (float)m_confirmBtnDisabledTex.width, (float)m_confirmBtnDisabledTex.height },
                m_confirmBtnBounds,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(m_confirmBtnBounds, GRAY);
        }
    } else {
        // Enabled state: ALWAYS draw normal button PNG (SeedChooser_Button.png)
        if (m_confirmBtnTex.id != 0) {
            DrawTexturePro(
                m_confirmBtnTex,
                { 0.0f, 0.0f, (float)m_confirmBtnTex.width, (float)m_confirmBtnTex.height },
                m_confirmBtnBounds,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(m_confirmBtnBounds, ORANGE);
        }

        // If hovered, overlay Glow PNG (SeedChooser_Button_Glow.png) on top
        if (isHovered && m_confirmBtnGlowTex.id != 0) {
            DrawTexturePro(
                m_confirmBtnGlowTex,
                { 0.0f, 0.0f, (float)m_confirmBtnGlowTex.width, (float)m_confirmBtnGlowTex.height },
                m_confirmBtnBounds,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }
    }

    // Centered "LET'S ROCK!" label on confirm button with Dwarven Todcraft font (m_font)
    Color labelColor = GRAY;
    if (isEnabled) {
        labelColor = isHovered ? GREEN : WHITE;
    }

    m_font.DrawTextCentered("LET'S ROCK!", m_confirmBtnBounds, 0.65f, labelColor);
}
