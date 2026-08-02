#pragma once
#include "raylib.h"
#include "resources.h"
#include <vector>
#include <string>

struct BowlingCard {
    float x;
    std::string plantType;
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

    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
};
