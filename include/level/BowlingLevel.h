#pragma once
#include "raylib.h"
#include "resources.h"
#include "Plant.h"
#include "Wallnut.h"
#include <vector>
#include <string>
#include <memory>

struct BowlingCard {
    float x;
    std::string plantType;
};

struct BowlingNut {
    float x;
    float y;
    int row;
    float rotationAngle;
    float rollSpeed;
    float rotationSpeed;
};

class BowlingLevel {
public:
    BowlingLevel(Resources& res, RenderTexture2D targetScreen);
    ~BowlingLevel() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    float m_animTimer = 0.0f;
    int m_currentFrame = 0;

    std::vector<BowlingCard> m_cards;
    float m_cardSpawnTimer = 0.0f;

    // Active rolling bowling nuts
    std::vector<BowlingNut> m_bowlingNuts;

    // Grid state for placed plants
    std::unique_ptr<Plant> m_grid[5][9];

    // Cursor card pickup state
    bool m_isHoldingCard = false;
    std::string m_heldPlantType = "";

    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
};
