#include "SeedBank.h"
#include <iostream>

SeedBank::SeedBank(int initialSun)
    : m_selectedPacketIndex(-1), m_sunCount(initialSun), m_isShovelSelected(false) {
    m_bankBounds = { 10.0f, 5.0f, 446.0f, 87.0f };
    m_shovelBounds = { 715.0f, 5.0f, 70.0f, 72.0f };
    initDefaultDeck();
}

void SeedBank::initDefaultDeck() {
    m_packets.clear();
    
    // Position of cards inside top SeedBank bar
    float startX = 88.0f;
    float startY = 10.0f;
    float cardW = 46.0f;
    float cardH = 68.0f;
    float spacing = 50.0f;

    struct DeckItem {
        std::string type;
        int cost;
        float cd;
        std::string tex;
    };

    std::vector<DeckItem> deck = {
        { "SunFlower", 50, 7.5f, "SUNFLOWER" },
        { "PeaShooter", 100, 7.5f, "PEASHOOTER" },
        { "Wallnut", 50, 30.0f, "WALLNUT" },
        { "SnowPea", 175, 7.5f, "SNOWPEA" },
        { "Repeater", 200, 7.5f, "REPEATER" },
        { "CherryBomb", 150, 35.0f, "CHERRYBOMB" },
        { "Jalapeno", 125, 35.0f, "JALAPENO" }
    };

    for (size_t i = 0; i < deck.size(); ++i) {
        Rectangle r = { startX + i * spacing, startY, cardW, cardH };
        m_packets.emplace_back(deck[i].type, deck[i].cost, deck[i].cd, deck[i].tex, r);
    }
}

void SeedBank::update(float dt, Vector2 mousePos, bool mouseClicked) {
    for (auto& packet : m_packets) {
        packet.update(dt);
    }

    if (mouseClicked) {
        // Check shovel click
        if (CheckCollisionPointRec(mousePos, m_shovelBounds)) {
            m_isShovelSelected = !m_isShovelSelected;
            m_selectedPacketIndex = -1;
            return;
        }

        // Check packet clicks
        bool packetClicked = false;
        for (size_t i = 0; i < m_packets.size(); ++i) {
            if (m_packets[i].isClicked(mousePos)) {
                if (m_packets[i].isReady(m_sunCount)) {
                    if (m_selectedPacketIndex == (int)i) {
                        m_selectedPacketIndex = -1; // Toggle off if clicked again
                    } else {
                        m_selectedPacketIndex = (int)i;
                        m_isShovelSelected = false;
                    }
                }
                packetClicked = true;
                break;
            }
        }
    }
}

void SeedBank::draw(Resources& res, Vector2 mousePos) const {
    // 1. Draw SeedBank background texture
    Texture2D bankTex = res.GetTexture("SEEDBANK");
    if (bankTex.id != 0) {
        DrawTexturePro(
            bankTex,
            { 0.0f, 0.0f, (float)bankTex.width, (float)bankTex.height },
            m_bankBounds,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(m_bankBounds, ColorAlpha(BROWN, 0.85f));
        DrawRectangleLinesEx(m_bankBounds, 2.0f, GOLD);
    }

    // 2. Draw Sun Counter Text inside Sun display area of SeedBank
    std::string sunStr = std::to_string(m_sunCount);
    int sunTextX = (int)m_bankBounds.x + 35 - MeasureText(sunStr.c_str(), 16) / 2;
    int sunTextY = (int)m_bankBounds.y + 60;
    DrawText(sunStr.c_str(), sunTextX + 1, sunTextY + 1, 16, BLACK);
    DrawText(sunStr.c_str(), sunTextX, sunTextY, 16, BLACK);

    // 3. Draw all Seed Packets inside top bar
    for (size_t i = 0; i < m_packets.size(); ++i) {
        m_packets[i].draw(res, m_sunCount, m_selectedPacketIndex == (int)i);
    }

    // 4. Draw Shovel Bank & Shovel
    Texture2D shovelBankTex = res.GetTexture("SHOVELBANK");
    if (shovelBankTex.id != 0) {
        DrawTexturePro(
            shovelBankTex,
            { 0.0f, 0.0f, (float)shovelBankTex.width, (float)shovelBankTex.height },
            m_shovelBounds,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(m_shovelBounds, ColorAlpha(DARKGRAY, 0.8f));
    }

    Texture2D shovelTex = res.GetTexture("SHOVEL");
    if (shovelTex.id != 0) {
        Rectangle shovelDrawRec = { m_shovelBounds.x + 3, m_shovelBounds.y + 4, 64.0f, 64.0f };
        DrawTexturePro(
            shovelTex,
            { 0.0f, 0.0f, (float)shovelTex.width, (float)shovelTex.height },
            shovelDrawRec,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
        if (m_isShovelSelected) {
            DrawRectangleLinesEx(shovelDrawRec, 2.5f, GOLD);
        }
    }

    // 5. Draw Cursor preview if a packet or shovel is selected
    if (m_selectedPacketIndex >= 0 && m_selectedPacketIndex < (int)m_packets.size()) {
        std::string texName = m_packets[m_selectedPacketIndex].getTextureName();
        Texture2D pTex = res.GetTexture(texName);
        if (pTex.id != 0) {
            Rectangle cursorRec = { mousePos.x - 25.0f, mousePos.y - 35.0f, 50.0f, 70.0f };
            DrawTexturePro(
                pTex,
                { 0.0f, 0.0f, (float)pTex.width, (float)pTex.height },
                cursorRec,
                { 0.0f, 0.0f },
                0.0f,
                ColorAlpha(WHITE, 0.8f)
            );
        }
    } else if (m_isShovelSelected && shovelTex.id != 0) {
        Rectangle cursorRec = { mousePos.x - 20.0f, mousePos.y - 20.0f, 40.0f, 40.0f };
        DrawTexturePro(
            shovelTex,
            { 0.0f, 0.0f, (float)shovelTex.width, (float)shovelTex.height },
            cursorRec,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
}

std::string SeedBank::getSelectedPlantType() const {
    if (m_selectedPacketIndex >= 0 && m_selectedPacketIndex < (int)m_packets.size()) {
        return m_packets[m_selectedPacketIndex].getPlantType();
    }
    return "";
}

int SeedBank::getSelectedPlantCost() const {
    if (m_selectedPacketIndex >= 0 && m_selectedPacketIndex < (int)m_packets.size()) {
        return m_packets[m_selectedPacketIndex].getSunCost();
    }
    return 0;
}

void SeedBank::deselect() {
    m_selectedPacketIndex = -1;
    m_isShovelSelected = false;
}

void SeedBank::consumeSelected() {
    if (m_selectedPacketIndex >= 0 && m_selectedPacketIndex < (int)m_packets.size()) {
        int cost = m_packets[m_selectedPacketIndex].getSunCost();
        m_sunCount -= cost;
        m_packets[m_selectedPacketIndex].triggerCooldown();
        m_selectedPacketIndex = -1;
    }
}

bool SeedBank::deductSun(int amount) {
    if (m_sunCount >= amount) {
        m_sunCount -= amount;
        return true;
    }
    return false;
}
