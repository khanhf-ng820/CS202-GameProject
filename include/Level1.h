#pragma once
#include "raylib.h"
#include "resources.h"
#include "SeedBank.h"
#include "Plant.h"
#include "Zombie.h"
#include "Projectile.h"
#include "SunItem.h"
#include <vector>
#include <memory>
#include <string>

class Level1 {
public:
    Level1(Resources& res, RenderTexture2D targetScreen);
    ~Level1() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    SeedBank m_seedBank;
    
    // 5x9 Lawn Grid for plant placement
    std::unique_ptr<Plant> m_grid[5][9];
    
    std::vector<std::unique_ptr<Zombie>> m_zombies;
    std::vector<Projectile> m_projectiles;
    std::vector<SunItem> m_suns;

    // Spawning & progression state
    float m_skySunTimer;
    float m_waveTimer;
    int m_currentWave;
    int m_maxWaves;
    bool m_levelWon;
    bool m_levelLost;
    bool m_finalWaveAnnounced;

    // Helper functions
    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
    void spawnSunFromSky();
    void spawnNextWave();
    void createPlant(const std::string& type, int row, int col, int pixelX, int pixelY);
    void updateCollisions(float dt);
};
