#pragma once
#include "raylib.h"
#include "resources.h"
#include "Plant.h"
#include "Wallnut.h"
#include "Zombie.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "FlagZombie.h"
#include "NewspaperZombie.h"
#include "FootballZombie.h"
#include "PoleVaultingZombie.h"
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

#include "BowlingNut.h"
#include "InGameMenu.h"
#include "BitmapFont.h"
#include "LawnMower.h"

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
    void restartLevel();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    float m_animTimer = 0.0f;
    int m_currentFrame = 0;

    // Intro "READY... SET... PLANT!" animation
    Reanimation m_readySetPlantAnim;
    float m_readySetPlantTimer = 0.0f;
    bool m_readySetPlantDone = false;

    std::vector<BowlingCard> m_cards;
    float m_cardSpawnTimer = 0.0f;

    // Active rolling bowling nuts
    std::vector<std::unique_ptr<BowlingNut>> m_bowlingNuts;

    // Active zombies
    std::vector<std::unique_ptr<Zombie>> m_zombies;
    std::vector<std::pair<const Zombie*, float>> m_hitDebugTimers;
    bool m_showDebug = false;

    // Lawn mowers on all 5 lanes
    std::vector<LawnMower> m_lawnMowers;

    // Level progression & wave state
    int m_currentWave = 0;
    int m_maxWaves = 10;
    float m_waveTimer = 2.0f; // 2 seconds initial delay before Wave 1
    bool m_finalWaveAnnounced = false;
    bool m_levelWon = false;
    bool m_levelLost = false;
    bool m_exitToMainMenu = false;
    float m_victoryDelayTimer = 0.0f;

    // Speed & Pause controls
    float m_gameSpeed = 1.0f;
    bool m_isSpeedPaused = false;

    // Defeat / Game Over presentation
    float m_loseTimer = 0.0f;
    bool m_screamSoundPlayed = false;
    bool m_loseMusicPlayed = false;

    // Victory award presentation
    float m_winTimer = 0.0f;
    float m_awardY = -100.0f;
    float m_awardRaysRotation = 0.0f;
    bool m_winMusicPlayed = false;

    // Grid state for placed plants
    std::unique_ptr<Plant> m_grid[5][9];

    // Cursor card pickup state
    bool m_isHoldingCard = false;
    std::string m_heldPlantType = "";

    // In-game pause menu
    std::unique_ptr<InGameMenu> m_inGameMenu;

    // Bitmap font for level label
    BitmapFont m_font;
    BitmapFont m_brianneFont;
    bool m_brianneLoaded = false;

    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
    void initLawnMowers();
    void spawnNextWave();
    void drawProgressBar();
    void drawSpeedControls();
    void drawLoseScreen();
    void drawWinScreen();
};

