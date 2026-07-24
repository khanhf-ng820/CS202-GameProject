#pragma once
#include "raylib.h"
#include "resources.h"
#include "SeedPacket.h"
#include "BitmapFont.h"
#include <vector>
#include <string>

class SeedBank {
private:
    std::vector<SeedPacket> m_packets;
    int m_selectedPacketIndex;
    int m_sunCount;
    bool m_isShovelSelected;
    Rectangle m_bankBounds;
    Rectangle m_shovelBounds;

    BitmapFont m_sunFont;
    BitmapFont m_priceFont;

public:
    SeedBank(int initialSun = 150);

    void initDefaultDeck();
    void update(float dt, Vector2 mousePos, bool mouseClicked);
    void draw(Resources& res, Vector2 mousePos) const;

    int getSelectedPacketIndex() const { return m_selectedPacketIndex; }
    std::string getSelectedPlantType() const;
    int getSelectedPlantCost() const;
    bool isShovelSelected() const { return m_isShovelSelected; }

    void deselect();
    void consumeSelected();
    
    int getSunCount() const { return m_sunCount; }
    void addSun(int amount) { m_sunCount += amount; }
    bool deductSun(int amount);
};
