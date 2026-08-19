#pragma once
#include "raylib.h"
#include "resources.h"
#include "Zombie.h"
#include "ZombieNormal.h"
#include "Reanimation.h"
#include <vector>
#include <memory>

class VasebreakerLevel {
public:
    VasebreakerLevel(Resources& res, RenderTexture2D targetScreen);
    ~VasebreakerLevel() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    // Mallet cursor animation state
    Reanimation m_malletAnim;
    bool m_isSwinging = false;

    // Active zombies on the lawn
    std::vector<std::unique_ptr<Zombie>> m_zombies;

    // Game state
    bool m_levelWon = false;
    bool m_levelLost = false;

    // Helper functions
    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
};
