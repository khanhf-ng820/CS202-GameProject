#pragma once
#include "raylib.h"
#include "resources.h"
#include "SeedBank.h"
#include "SeedSelectMenu.h"
#include "Plant.h"
#include "Zombie.h"
#include "particle.h"
#include "Projectile.h"
#include "SunItem.h"
#include "InGameMenu.h"
#include "Reanimation.h"
#include "LawnMower.h"
#include "BitmapFont.h"
#include <vector>
#include <memory>
#include <string>

enum class LevelPhase {
    SeedSelection,
    PanToLawn,
    ReadySetPlant,
    ActiveWave
};

class Level1 {
public:
    Level1(Resources& res, RenderTexture2D targetScreen, int levelNumber = 1);
    virtual ~Level1() = default;

    void run();
    void update(float dt);
    void draw();
    virtual void restartLevel();

protected:
    Resources& res;
    RenderTexture2D targetScreen;
    int m_levelNumber = 1;
    BitmapFont m_font;

    LevelPhase m_phase;
    SeedSelectMenu m_seedSelectMenu;
    SeedBank m_seedBank;
    
    // Camera pan tracking
    float m_cameraCropX = 500.0f;
    float m_panTimer = 0.0f;
    float m_panDuration = 2.5f;

    // Intro "READY... SET... PLANT!" animation
    Reanimation m_readySetPlantAnim;
    float m_readySetPlantTimer = 0.0f;

    // 5x9 Lawn Grid for plant placement
    std::unique_ptr<Plant> m_grid[5][9];
    
    struct PreviewZombieItem {
        std::unique_ptr<Zombie> zombie;
        float worldX;
        float worldY;
    };

    std::vector<std::unique_ptr<Zombie>> m_zombies;
    std::vector<PreviewZombieItem> m_previewZombies;
    std::vector<Projectile> m_projectiles;
    std::vector<SunItem> m_suns;
    std::vector<ParticleEffect> m_effects;
    std::vector<LawnMower> m_lawnMowers;

    // Spawning & progression state
    float m_skySunTimer;
    float m_waveTimer;
    int m_currentWave;
    int m_maxWaves;
    bool m_levelWon;
    bool m_levelLost;
    bool m_finalWaveAnnounced;
    bool m_exitToMainMenu;

    // Speed & Pause controls
    float m_gameSpeed = 1.0f;
    bool m_isSpeedPaused = false;

    // Victory award presentation
    float m_winTimer = 0.0f;
    float m_awardY = -100.0f;
    float m_awardRaysRotation = 0.0f;
    bool m_winMusicPlayed = false;

    // Defeat / Game Over presentation
    float m_loseTimer = 0.0f;
    bool m_screamSoundPlayed = false;
    bool m_loseMusicPlayed = false;

    // In-game pause menu
    std::unique_ptr<InGameMenu> m_inGameMenu;
    bool m_ignoreInitialClick = true;

    // Helper functions & Virtual hooks for derived levels
    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
    virtual std::vector<std::string> getUniqueLevelZombieTypes() const;
    void initPreviewZombies();
    void initLawnMowers();
    void spawnSunFromSky();
    virtual void spawnNextWave();
    void createPlant(const std::string& type, int row, int col, int pixelX, int pixelY);
    void createSplat(float x, float y, bool isSnow);
    void createFireSplat(float x, float y);
    void createEatingParticle(float x, float y);
    void updateCollisions(float dt);
    void drawProgressBar();
    void drawSpeedControls();
    void drawWinScreen();
    void drawLoseScreen();
};
