#pragma once
#include "raylib.h"
#include "resources.h"

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

    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
};
