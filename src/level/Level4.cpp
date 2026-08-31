#include "Level4.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include "ProfileManager.h"
#include "PeaShooter.h"
#include "SunFlower.h"
#include "Wallnut.h"
#include "SnowPea.h"
#include "Repeater.h"
#include "CherryBomb.h"
#include "Chomper.h"
#include "Jalapeno.h"
#include "FirePea.h"
#include "GatlingPea.h"
#include "Cornpult.h"
#include "Melonpult.h"
#include "Torchwood.h"
#include "PotatoMine.h"
#include "Squash.h"
#include "Cabbagepult.h"
#include "IceShroom.h"
#include "Gravebuster.h"
#include "Garlic.h"
#include "Caltrop.h"
#include "SpikeRock.h"
#include "Plantern.h"
#include "ZombieNormal.h"
#include "FlagZombie.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "NewspaperZombie.h"
#include "FootballZombie.h"
#include "PoleVaultingZombie.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cmath>

Level4::Level4(Resources& res, RenderTexture2D targetScreen, int levelNumber)
    : res(res), targetScreen(targetScreen),
      m_levelNumber(levelNumber), m_hasFog(false), m_fogTimer(0.0f), m_fogStartX(480.0f),
      m_phase(LevelPhase::SeedSelection),
      m_seedSelectMenu(res), m_seedBank(40000),
      m_waveTimer(14.0f), m_currentWave(0),
      m_maxWaves(8), m_levelWon(false), m_levelLost(false),
      m_finalWaveAnnounced(false), m_exitToMainMenu(false),
      m_gameSpeed(1.0f), m_isSpeedPaused(false),
      m_winTimer(0.0f), m_awardY(-150.0f), m_awardRaysRotation(0.0f), m_winMusicPlayed(false),
      m_loseTimer(0.0f), m_screamSoundPlayed(false), m_loseMusicPlayed(false),
      m_introGraveSoundPlayed(false)
{
    m_inGameMenu = std::make_unique<InGameMenu>(res);

    // Load House of Terror bitmap font for level labels
    m_font.Load(res.GetAssetPath("assets/data/HouseofTerror28.png"), res.GetAssetPath("assets/data/HouseofTerror28.txt"));

    // Initialize "READY... SET... PLANT!" intro animation
    ReanimDefinition readyDef = res.LoadReanim(res.GetAssetPath("assets/reanim/StartReadySetPlant.reanim"));
    m_readySetPlantAnim.SetResources(readyDef, res);
    m_readySetPlantAnim.SetLooping(false);

    m_texBgNight = res.GetTexture("BACKGROUND2");
    if (m_texBgNight.id == 0) m_texBgNight = res.GetTexture("background2");

    m_texTombstones = res.GetTexture("TOMBSTONES");
    if (m_texTombstones.id == 0) m_texTombstones = res.GetTexture("Tombstones");

    m_texMounds = res.GetTexture("TOMBSTONE_MOUNDS");
    if (m_texMounds.id == 0) m_texMounds = res.GetTexture("Tombstone_mounds");

    m_texFog = res.GetTexture("FOG");
    if (m_texFog.id == 0) m_texFog = res.GetTexture("fog");

    // Clear grid
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
        m_rowFogStartX[r] = m_baseFogStartX;
        m_targetRowFogStartX[r] = m_baseFogStartX;
    }

    initGraves();
    initPreviewZombies();
    initLawnMowers();
}

void Level4::initGraves() {
    m_graves.clear();
    m_introGraveSoundPlayed = false;
    // Default tombstones on cols 6..8
    m_graves.push_back({ 0, 7, GetRandomValue(0, 4), 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 1, 6, GetRandomValue(0, 4), 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 2, 8, GetRandomValue(0, 4), 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 3, 7, GetRandomValue(0, 4), 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 4, 6, GetRandomValue(0, 4), 0, false, 0.0f, 0.0f, "" });
}

bool Level4::isCellBlockedByGrave(int row, int col) const {
    for (const auto& g : m_graves) {
        if (!g.isDestroyed && g.row == row && g.col == col) {
            return true;
        }
    }
    return false;
}

void Level4::restartLevel() {
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
        m_rowFogStartX[r] = m_baseFogStartX;
        m_targetRowFogStartX[r] = m_baseFogStartX;
    }

    m_zombies.clear();
    m_risingZombies.clear();
    m_projectiles.clear();
    m_suns.clear();
    m_effects.clear();

    m_currentWave = 0;
    m_waveTimer = 14.0f;
    m_levelWon = false;
    m_levelLost = false;
    m_finalWaveAnnounced = false;
    m_exitToMainMenu = false;

    m_gameSpeed = 1.0f;
    m_isSpeedPaused = false;
    m_winTimer = 0.0f;
    m_awardY = -150.0f;
    m_awardRaysRotation = 0.0f;
    m_winMusicPlayed = false;
    m_loseTimer = 0.0f;
    m_screamSoundPlayed = false;
    m_loseMusicPlayed = false;

    m_phase = LevelPhase::SeedSelection;
    m_cameraCropX = 500.0f;
    m_panTimer = 0.0f;
    m_readySetPlantTimer = 0.0f;
    m_seedBank.deselect();
    m_ignoreInitialClick = true;
    AudioManager::GetInstance().PlayMusic(MusicTrack::None);

    initGraves();
    initPreviewZombies();
    initLawnMowers();
}

std::vector<std::string> Level4::getUniqueLevelZombieTypes() const {
    return {
        "ZombieNormal",
        "ConeheadZombie",
        "BucketheadZombie",
        "NewspaperZombie",
        "FlagZombie"
    };
}

void Level4::initPreviewZombies() {
    m_previewZombies.clear();
    std::vector<std::string> uniqueTypes = getUniqueLevelZombieTypes();

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    std::vector<int> availableRows = { 0, 1, 2, 3, 4 };
    for (size_t i = 0; i < uniqueTypes.size(); ++i) {
        std::string typeName = uniqueTypes[i];
        int row = availableRows[i % availableRows.size()];

        float worldX = 1130.0f + (float)(GetRandomValue(0, 180));
        float worldY = laneY(row) + (float)(GetRandomValue(-10, 10));

        std::unique_ptr<Zombie> previewZ;
        if (typeName == "ZombieNormal") {
            previewZ = std::make_unique<ZombieNormal>(res, worldX, worldY);
        } else if (typeName == "ConeheadZombie") {
            previewZ = std::make_unique<ConeheadZombie>(res, worldX, worldY);
        } else if (typeName == "BucketheadZombie") {
            previewZ = std::make_unique<BucketheadZombie>(res, worldX, worldY);
        } else if (typeName == "NewspaperZombie") {
            previewZ = std::make_unique<NewspaperZombie>(res, worldX, worldY);
        } else if (typeName == "PoleVaultingZombie") {
            previewZ = std::make_unique<PoleVaultingZombie>(res, worldX, worldY);
        } else if (typeName == "FootballZombie") {
            previewZ = std::make_unique<FootballZombie>(res, worldX, worldY);
        } else if (typeName == "FlagZombie") {
            previewZ = std::make_unique<FlagZombie>(res, worldX, worldY);
        } else {
            previewZ = std::make_unique<ZombieNormal>(res, worldX, worldY);
        }

        if (previewZ) {
            previewZ->getAnim().SetBaseAnimation("anim_idle");
            previewZ->getAnim().SetAnimation("anim_idle");
            m_previewZombies.push_back({ std::move(previewZ), worldX, worldY });
        }
    }

    std::sort(m_previewZombies.begin(), m_previewZombies.end(),
        [](const PreviewZombieItem& a, const PreviewZombieItem& b) {
            return a.worldY < b.worldY;
        });
}

void Level4::initLawnMowers() {
    m_lawnMowers.clear();
    for (int r = 0; r < 5; ++r) {
        float mowerX = 65.0f;
        float mowerY = 80.0f + (float)r * 100.0f;
        m_lawnMowers.emplace_back(res, mowerX, mowerY, r);
    }
}

bool Level4::getGridCell(Vector2 mousePos, int& outRow, int& outCol) const {
    float startX = 140.0f;
    float startY = 80.0f;

    if (mousePos.x < startX || mousePos.y < startY) return false;

    int row = (int)((mousePos.y - startY) / 100.0f);
    if (row < 0 || row >= 5) return false;

    float relX = mousePos.x - startX;
    int col = (relX < 80.0f) ? 0 : 1 + (int)((relX - 80.0f) / 70.0f);

    if (col >= 0 && col < 9) {
        outRow = row;
        outCol = col;
        return true;
    }
    return false;
}

void Level4::createGraveDirtParticle(float x, float y, int count) {
    for (int i = 0; i < count; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(-25, 25);
        p.y = y + (float)GetRandomValue(-15, 15);
        p.vx = (float)GetRandomValue(-180, 180);
        p.vy = (float)GetRandomValue(-320, -90);
        p.gravity = 600.0f;
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-600, 600);
        p.scale = (float)GetRandomValue(12, 22) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 1.4f;
        p.isPhysicsParticle = true;
        p.active = true;

        int c = GetRandomValue(0, 3);
        if (c == 0) p.tint = Color{ 110, 75, 45, 255 };
        else if (c == 1) p.tint = Color{ 60, 40, 20, 255 };
        else if (c == 2) p.tint = Color{ 160, 120, 70, 255 };
        else p.tint = Color{ 85, 60, 35, 255 };

        m_effects.push_back(p);
    }
}

void Level4::createGraveCrumbleParticles(float x, float y) {
    int rockCount = GetRandomValue(20, 28);
    for (int i = 0; i < rockCount; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(-25, 25);
        p.y = y + (float)GetRandomValue(-25, 25);
        p.vx = (float)GetRandomValue(-220, 220);
        p.vy = (float)GetRandomValue(-360, -120);
        p.gravity = 650.0f;
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-720, 720);
        p.scale = (float)GetRandomValue(14, 26) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 1.5f;
        p.isPhysicsParticle = true;
        p.active = true;

        int c = GetRandomValue(0, 2);
        if (c == 0) p.tint = Color{ 145, 145, 150, 255 };
        else if (c == 1) p.tint = Color{ 95, 95, 100, 255 };
        else p.tint = Color{ 180, 180, 185, 255 };

        m_effects.push_back(p);
    }

    createGraveDirtParticle(x, y, 25);
}

void Level4::triggerGraveRising(int count) {
    std::vector<int> activeIndices;
    for (size_t i = 0; i < m_graves.size(); ++i) {
        if (!m_graves[i].isDestroyed && m_graves[i].shakeTimer <= 0.0f) {
            activeIndices.push_back((int)i);
        }
    }

    if (activeIndices.empty()) return;

    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/gravestone_rumble.ogg"));

    if (count <= 0 || count >= (int)activeIndices.size()) {
        for (int idx : activeIndices) {
            auto& g = m_graves[idx];
            g.shakeTimer = 0.8f;
            if (m_currentWave >= 6 && GetRandomValue(0, 2) == 0) {
                g.pendingZombieType = "BucketheadZombie";
            } else if (m_currentWave >= 3 && GetRandomValue(0, 1) == 0) {
                g.pendingZombieType = "ConeheadZombie";
            } else {
                g.pendingZombieType = "ZombieNormal";
            }
        }
    } else {
        std::vector<int> shuffled = activeIndices;
        for (int i = (int)shuffled.size() - 1; i > 0; --i) {
            int j = GetRandomValue(0, i);
            std::swap(shuffled[i], shuffled[j]);
        }
        for (int i = 0; i < count && i < (int)shuffled.size(); ++i) {
            auto& g = m_graves[shuffled[i]];
            g.shakeTimer = 0.8f;
            if (m_currentWave >= 6 && GetRandomValue(0, 2) == 0) {
                g.pendingZombieType = "BucketheadZombie";
            } else if (m_currentWave >= 3 && GetRandomValue(0, 1) == 0) {
                g.pendingZombieType = "ConeheadZombie";
            } else {
                g.pendingZombieType = "ZombieNormal";
            }
        }
    }
}

void Level4::spawnNextWave() {
    m_currentWave++;
    float spawnX = 700.0f;

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    bool isHugeWave = false;

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        triggerGraveRising(1);
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(3)));
        triggerGraveRising(1);
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
        triggerGraveRising(2);
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(2)));
        triggerGraveRising(2);
    } else if (m_currentWave == 5) {
        isHugeWave = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
        triggerGraveRising(-1); // ALL graves emerge!
    } else if (m_currentWave == 6) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        triggerGraveRising(2);
    } else if (m_currentWave == 7) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(1)));
        triggerGraveRising(3);
    } else if (m_currentWave == 8) {
        isHugeWave = true;
        m_finalWaveAnnounced = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
        triggerGraveRising(-1); // ALL graves emerge!
    }

    if (isHugeWave) {
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));
    } else {
        static const std::vector<std::string> waveGroanSounds = {
            "assets/sounds/sukhbir.ogg", "assets/sounds/sukhbir2.ogg", "assets/sounds/sukhbir3.ogg",
            "assets/sounds/groan.ogg", "assets/sounds/groan2.ogg", "assets/sounds/lowgroan.ogg"
        };
        int rIdx = GetRandomValue(0, (int)waveGroanSounds.size() - 1);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(waveGroanSounds[rIdx]));
    }
}

void Level4::createPlant(const std::string& type, int row, int col, int pixelX, int pixelY) {
    if (type == "PeaShooter") {
        m_grid[row][col] = std::make_unique<PeaShooter>(res, pixelX, pixelY);
    } else if (type == "SunFlower") {
        m_grid[row][col] = std::make_unique<SunFlower>(res, pixelX, pixelY);
    } else if (type == "Wallnut") {
        m_grid[row][col] = std::make_unique<Wallnut>(res, pixelX, pixelY);
    } else if (type == "SnowPea") {
        m_grid[row][col] = std::make_unique<SnowPea>(res, pixelX, pixelY);
    } else if (type == "Repeater") {
        m_grid[row][col] = std::make_unique<Repeater>(res, pixelX, pixelY);
    } else if (type == "CherryBomb") {
        m_grid[row][col] = std::make_unique<CherryBomb>(res, pixelX, pixelY);
    } else if (type == "Chomper") {
        m_grid[row][col] = std::make_unique<Chomper>(res, pixelX, pixelY);
    } else if (type == "Jalapeno") {
        m_grid[row][col] = std::make_unique<Jalapeno>(res, pixelX, pixelY);
    } else if (type == "FirePea") {
        m_grid[row][col] = std::make_unique<FirePea>(res, pixelX, pixelY);
    } else if (type == "GatlingPea") {
        m_grid[row][col] = std::make_unique<GatlingPea>(res, pixelX, pixelY);
    } else if (type == "Cornpult") {
        m_grid[row][col] = std::make_unique<Cornpult>(res, pixelX, pixelY);
    } else if (type == "Melonpult") {
        m_grid[row][col] = std::make_unique<Melonpult>(res, pixelX, pixelY);
    } else if (type == "Torchwood") {
        m_grid[row][col] = std::make_unique<Torchwood>(res, pixelX, pixelY);
    } else if (type == "PotatoMine") {
        m_grid[row][col] = std::make_unique<PotatoMine>(res, pixelX, pixelY);
    } else if (type == "Squash") {
        m_grid[row][col] = std::make_unique<Squash>(res, pixelX, pixelY);
    } else if (type == "Cabbagepult") {
        m_grid[row][col] = std::make_unique<Cabbagepult>(res, pixelX, pixelY);
    } else if (type == "IceShroom") {
        m_grid[row][col] = std::make_unique<IceShroom>(res, pixelX, pixelY);
    } else if (type == "Gravebuster") {
        m_grid[row][col] = std::make_unique<Gravebuster>(res, pixelX, pixelY);
    } else if (type == "Garlic") {
        m_grid[row][col] = std::make_unique<Garlic>(res, pixelX, pixelY);
    } else if (type == "Caltrop") {
        m_grid[row][col] = std::make_unique<Caltrop>(res, pixelX, pixelY);
    } else if (type == "SpikeRock") {
        m_grid[row][col] = std::make_unique<SpikeRock>(res, pixelX, pixelY);
    } else if (type == "Plantern") {
        m_grid[row][col] = std::make_unique<Plantern>(res, pixelX, pixelY);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/plantern.ogg"));
    }
}

void Level4::createSplat(float x, float y, bool isSnow) {
    ParticleEffect eff;
    if (isSnow) {
        eff.texture = res.GetTexture("SNOWPEA_SPLATS");
        if (eff.texture.id == 0) eff.texture = res.GetTexture("SnowPea_splats");
        eff.totalFrames = 4;
    } else {
        eff.texture = res.GetTexture("PEA_SPLATS");
        if (eff.texture.id == 0) eff.texture = res.GetTexture("pea_splats");
        eff.totalFrames = 4;
    }
    if (eff.texture.id == 0) return;

    eff.x = x;
    eff.y = y;
    eff.currentFrame = 0;
    eff.frameDuration = 0.05f;
    eff.timer = 0.0f;
    eff.scale = 1.2f;
    eff.active = true;
    m_effects.push_back(eff);
}

void Level4::createFireSplat(float x, float y) {
    Texture2D sparkTex = res.GetTexture("FirePea_spark");
    if (sparkTex.id == 0) sparkTex = res.GetTexture("FIREPEA_SPARK");

    int sparkCount = GetRandomValue(10, 14);
    for (int i = 0; i < sparkCount; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(-5, 5);
        p.y = y + (float)GetRandomValue(-5, 5);
        p.vx = (float)GetRandomValue(-160, 160);
        p.vy = (float)GetRandomValue(-200, 30);
        p.gravity = 450.0f;
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-600, 600);
        p.scale = (float)GetRandomValue(9, 18) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 3.2f;
        p.isPhysicsParticle = true;
        p.active = true;
        if (sparkTex.id != 0) {
            p.texture = sparkTex;
        }

        int c = GetRandomValue(0, 2);
        if (c == 0) p.tint = Color{ 255, 230, 80, 255 };
        else if (c == 1) p.tint = Color{ 255, 140, 20, 255 };
        else p.tint = Color{ 255, 50, 10, 255 };

        m_effects.push_back(p);
    }
}

void Level4::createEatingParticle(float x, float y) {
    int count = GetRandomValue(2, 4);
    for (int i = 0; i < count; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(0, 20);
        p.y = y + (float)GetRandomValue(-15, 10);
        p.vx = (float)GetRandomValue(-55, 55);
        p.vy = (float)GetRandomValue(-120, -40);
        p.gravity = 500.0f;
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-360, 360);
        p.scale = (float)GetRandomValue(8, 14) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 2.5f;
        p.isPhysicsParticle = true;
        p.active = true;

        int c = GetRandomValue(0, 2);
        if (c == 0) p.tint = Color{ 34, 177, 76, 255 };
        else if (c == 1) p.tint = Color{ 140, 210, 40, 255 };
        else p.tint = Color{ 20, 120, 40, 255 };

        m_effects.push_back(p);
    }
}

void Level4::updateCollisions(float dt) {
    // 1. Projectiles vs Zombies
    for (auto& p : m_projectiles) {
        if (!p.isActive() || p.isImpacting()) continue;
        for (auto& z : m_zombies) {
            if (z->isDead()) continue;
            float projBaseY = p.isLobbed() ? (p.getStartY() + 60.0f) : p.getY();
            if (std::abs(projBaseY - (z->getY() + 40.0f)) < 55.0f) {
                if (p.getX() >= z->getX() + 5.0f && p.getX() <= z->getX() + 45.0f) {
                    if (p.isMelon()) {
                        static const std::vector<std::string> melonHitSounds = {
                            "assets/sounds/melonimpact.ogg",
                            "assets/sounds/melonimpact2.ogg"
                        };
                        int idx = GetRandomValue(0, (int)melonHitSounds.size() - 1);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(melonHitSounds[idx]));
                    } else if (p.isButter()) {
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/butter.ogg"));
                    } else if (z->getName() == "BucketheadZombie" && z->getHp() > 200) {
                        static const std::vector<std::string> shieldSounds = {
                            "assets/sounds/shieldhit.ogg",
                            "assets/sounds/shieldhit2.ogg"
                        };
                        int idx = GetRandomValue(0, (int)shieldSounds.size() - 1);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(shieldSounds[idx]));
                    } else if (z->getName() == "ConeheadZombie" && z->getHp() > 200) {
                        static const std::vector<std::string> plasticSounds = {
                            "assets/sounds/plastichit.ogg",
                            "assets/sounds/plastichit2.ogg"
                        };
                        int idx = GetRandomValue(0, (int)plasticSounds.size() - 1);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(plasticSounds[idx]));
                    } else {
                        static const std::vector<std::string> splatSounds = {
                            "assets/sounds/splat.ogg",
                            "assets/sounds/splat2.ogg",
                            "assets/sounds/splat3.ogg"
                        };
                        int idx = GetRandomValue(0, (int)splatSounds.size() - 1);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(splatSounds[idx]));
                    }

                    z->takeDamage(p.getDamage());
                    if (p.isSnow()) {
                        z->applySlow(3.0f);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/frozen.ogg"));
                    }

                    if (p.isMelon()) {
                        for (auto& otherZ : m_zombies) {
                            if (otherZ.get() == z.get() || otherZ->isDead()) continue;
                            float dx = otherZ->getX() - z->getX();
                            float dy = otherZ->getY() - z->getY();
                            if (dx * dx + dy * dy <= 120.0f * 120.0f) {
                                otherZ->takeDamage(26);
                            }
                        }
                    }

                    p.onHit();
                    break;
                }
            }
        }
    }

    // 1.5. Projectiles vs Torchwood
    for (auto& p : m_projectiles) {
        if (!p.isActive() || p.isFire() || p.isLobbed()) continue;
        int pRow = (int)((p.getY() - 80.0f + 40.0f) / 100.0f);
        if (pRow >= 0 && pRow < 5) {
            int pCol = (int)((p.getX() - 140.0f + 35.0f) / 70.0f);
            if (pCol >= 0 && pCol < 9) {
                Plant* plant = m_grid[pRow][pCol].get();
                if (plant && plant->getName() == "Torchwood" && !plant->isDead()) {
                    if (p.getLastTorchwoodCol() != pCol) {
                        if (p.isSnow()) {
                            p.melt();
                            p.setLastTorchwoodCol(pCol);
                            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/ignite.ogg"));
                        } else {
                            p.setFire(true);
                            p.setLastTorchwoodCol(pCol);
                            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/ignite.ogg"));
                        }
                    }
                }
            }
        }
    }

    // 1.8. Grid Plant Triggers vs Zombies & Graves
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            Plant* plant = m_grid[r][c].get();
            if (!plant || plant->isDead()) continue;

            if (plant->getName() == "Jalapeno") {
                Jalapeno* jal = dynamic_cast<Jalapeno*>(plant);
                if (jal && jal->isExplodingFire() && !jal->hasDealtDamage()) {
                    jal->markDamageDealt();
                    for (auto& z : m_zombies) {
                        if (z->isDead() || z->isDevoured()) continue;
                        int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                        if (zRow == r) {
                            z->takeExplosiveDamage(1800);
                        }
                    }
                }
            } else if (plant->getName() == "CherryBomb") {
                CherryBomb* cb = dynamic_cast<CherryBomb*>(plant);
                if (cb && cb->isExplodingEffect() && !cb->hasDealtDamage()) {
                    cb->markDamageDealt();
                    float cbCx = (float)cb->getX() + 40.0f;
                    float cbCy = (float)cb->getY() + 40.0f;
                    for (auto& z : m_zombies) {
                        if (z->isDead() || z->isDevoured()) continue;
                        float zCx = z->getX() + 40.0f;
                        float zCy = z->getY() + 40.0f;
                        float dx = zCx - cbCx;
                        float dy = zCy - cbCy;
                        if (dx * dx + dy * dy <= 180.0f * 180.0f) {
                            z->takeExplosiveDamage(1800);
                        }
                    }
                }
            } else if (plant->getName() == "PotatoMine") {
                PotatoMine* pm = dynamic_cast<PotatoMine*>(plant);
                if (pm) {
                    if (pm->isArmed()) {
                        float pmCx = (float)pm->getX() + 40.0f;
                        for (auto& z : m_zombies) {
                            if (z->isDead() || z->isDevoured()) continue;
                            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                            if (zRow == r) {
                                float zCx = z->getX() + 40.0f;
                                if (std::abs(zCx - pmCx) < 45.0f) {
                                    pm->triggerExplode();
                                    break;
                                }
                            }
                        }
                    } else if (pm->isExploding() && !pm->hasDealtDamage()) {
                        pm->markDamageDealt();
                        float pmCx = (float)pm->getX() + 40.0f;
                        for (auto& z : m_zombies) {
                            if (z->isDead() || z->isDevoured()) continue;
                            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                            if (zRow == r) {
                                float zCx = z->getX() + 40.0f;
                                if (std::abs(zCx - pmCx) < 70.0f) {
                                    z->takeExplosiveDamage(1800);
                                }
                            }
                        }
                    }
                }
            } else if (plant->getName() == "Squash") {
                Squash* sq = dynamic_cast<Squash*>(plant);
                if (sq) {
                    if (sq->isIdle()) {
                        float sqCx = (float)sq->getX() + 40.0f;
                        for (auto& z : m_zombies) {
                            if (z->isDead() || z->isDevoured()) continue;
                            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                            if (zRow == r) {
                                float zCx = z->getX() + 40.0f;
                                if (zCx >= sqCx - 60.0f && zCx <= sqCx + 100.0f) {
                                    sq->setTargetZombie(z->getX(), z->getY());
                                    break;
                                }
                            }
                        }
                    } else if (sq->isSquashing() && !sq->hasDealtDamage()) {
                        sq->markDamageDealt();
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/gargantuar_thump.ogg"));
                        float sqCx = sq->getTargetX() + 40.0f;
                        for (auto& z : m_zombies) {
                            if (z->isDead() || z->isDevoured()) continue;
                            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                            if (zRow == r) {
                                float zCx = z->getX() + 40.0f;
                                if (std::abs(zCx - sqCx) < 80.0f) {
                                    z->takeSquashDamage(1800);
                                }
                            }
                        }
                    }
                }
            } else if (plant->getName() == "IceShroom") {
                IceShroom* ice = dynamic_cast<IceShroom*>(plant);
                if (ice && ice->isFreezing() && !ice->hasDealtDamage()) {
                    ice->markDamageDealt();
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/frozen.ogg"));
                    for (auto& z : m_zombies) {
                        if (!z->isDead() && !z->isDevoured()) {
                            z->takeDamage(20);
                            z->applySlow(6.0f);
                        }
                    }
                }
            } else if (plant->getName() == "Caltrop" || plant->getName() == "SpikeRock") {
                float plantCx = (float)plant->getX() + 40.0f;
                bool zombieOnTop = false;
                for (auto& z : m_zombies) {
                    if (z->isDead() || z->isDevoured()) continue;
                    int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                    if (zRow == r) {
                        float zCx = z->getX() + 40.0f;
                        if (std::abs(zCx - plantCx) < 50.0f) {
                            zombieOnTop = true;
                            float spikeDmg = (plant->getName() == "SpikeRock") ? 50.0f : 25.0f;
                            z->takeDamage(spikeDmg * dt);
                        }
                    }
                }
                if (plant->getName() == "Caltrop") {
                    Caltrop* cal = dynamic_cast<Caltrop*>(plant);
                    if (cal) cal->setAttacking(zombieOnTop);
                } else if (plant->getName() == "SpikeRock") {
                    SpikeRock* sr = dynamic_cast<SpikeRock*>(plant);
                    if (sr) sr->setAttacking(zombieOnTop);
                }
            } else if (plant->getName() == "Chomper") {
                Chomper* ch = dynamic_cast<Chomper*>(plant);
                if (ch && ch->isIdle()) {
                    float chCx = (float)ch->getX() + 40.0f;
                    for (auto& z : m_zombies) {
                        if (z->isDead() || z->isDevoured()) continue;
                        int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                        if (zRow == r) {
                            float zCx = z->getX() + 40.0f;
                            if (zCx >= chCx - 20.0f && zCx <= chCx + 85.0f) {
                                ch->devourZombie(z.get());
                                break;
                            }
                        }
                    }
                }
            } else if (plant->getName() == "Gravebuster") {
                Gravebuster* gb = dynamic_cast<Gravebuster*>(plant);
                if (gb) {
                    for (auto& g : m_graves) {
                        if (!g.isDestroyed && g.row == r && g.col == c) {
                            float progress = gb->getEatProgress();
                            if (GetRandomValue(0, 3) == 0) {
                                float cellX = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f);
                                float cellY = 80.0f + g.row * 100.0f;
                                createGraveDirtParticle(cellX + 35.0f, cellY + 45.0f + progress * 25.0f, 2);
                            }

                            if (gb->isFinished()) {
                                g.isDestroyed = true;
                                float cellX = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f);
                                float cellY = 80.0f + g.row * 100.0f;
                                createGraveCrumbleParticles(cellX + 35.0f, cellY + 75.0f);
                                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/dirt_rise.ogg"));
                                Texture2D texSun = res.GetTexture("SUN");
                                if (texSun.id == 0) texSun = res.GetTexture("Sun");
                                m_suns.emplace_back(cellX + 25.0f, cellY + 15.0f, texSun);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    // 2. Zombies Eating Plants & House Breach
    for (auto& z : m_zombies) {
        if (z->isDead() || z->isSquashed()) continue;

        PoleVaultingZombie* pvz = dynamic_cast<PoleVaultingZombie*>(z.get());
        if (pvz && pvz->isVaulting()) {
            continue;
        }

        bool foundPlantToEat = false;
        int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
        if (zRow < 0) zRow = 0;
        if (zRow > 4) zRow = 4;

        for (int c = 0; c < 9; ++c) {
            Plant* p = m_grid[zRow][c].get();
            if (p && !p->isDead()) {
                if (p->getName() == "Caltrop" || p->getName() == "SpikeRock") {
                    continue;
                }

                float plantX = (float)p->getX();

                if (pvz && !pvz->hasVaulted() && !pvz->isVaulting()) {
                    if (z->getX() >= plantX - 10.0f && z->getX() <= plantX + 80.0f) {
                        pvz->startVault();
                        break;
                    }
                }

                if (z->getX() >= plantX - 20.0f && z->getX() <= plantX + 45.0f) {
                    if (p->getName() == "Garlic") {
                        p->takeDamage(25.0f);
                        int nextRow = zRow;
                        if (zRow == 0) nextRow = 1;
                        else if (zRow == 4) nextRow = 3;
                        else nextRow = (std::rand() % 2 == 0) ? (zRow - 1) : (zRow + 1);

                        z->setY(45.0f + nextRow * 100.0f);
                        z->setEating(false);
                        z->resetEatTimer();
                        z->getAnim().SetAnimation("anim_walk");
                        foundPlantToEat = false;

                        if (p->isDead()) {
                            m_grid[zRow][c] = nullptr;
                        }
                        break;
                    }

                    foundPlantToEat = true;
                    z->setEating(true);
                    if (z->getAnim().GetCurrentAnimName() != "anim_eat") {
                        z->getAnim().SetAnimation("anim_eat");
                    }

                    p->takeDamage((float)z->getDamage() * dt);

                    z->addEatTimer(dt);
                    if (z->getEatTimer() >= 0.28f) {
                        z->resetEatTimer();
                        createEatingParticle(plantX + 25.0f, (float)p->getY() + 45.0f);
                        static const std::vector<std::string> chompSounds = {
                            "assets/sounds/chomp.ogg",
                            "assets/sounds/chomp2.ogg",
                            "assets/sounds/chompsoft.ogg"
                        };
                        int chompIdx = GetRandomValue(0, (int)chompSounds.size() - 1);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(chompSounds[chompIdx]));
                    }

                    if (p->isDead()) {
                        m_grid[zRow][c] = nullptr;
                        z->setEating(false);
                        z->resetEatTimer();
                        z->getAnim().SetAnimation("anim_walk");
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/gulp.ogg"));
                    }
                    break;
                }
            }
        }

        if (!foundPlantToEat && (!pvz || (!pvz->isVaulting() && pvz->hasVaulted()))) {
            if (z->isEating() || z->getAnim().GetCurrentAnimName() == "anim_eat") {
                z->setEating(false);
                z->resetEatTimer();
                z->getAnim().SetAnimation("anim_walk");
            }
        }

        // LawnMower check
        for (auto& mower : m_lawnMowers) {
            if (mower.getRow() == zRow && !mower.isFinished()) {
                if (!mower.isTriggered() && z->getX() <= mower.getX() + 30.0f) {
                    mower.trigger();
                }
            }
        }

        // House Breach
        if (z->getX() <= 20.0f) {
            bool rowHasMower = false;
            for (const auto& mower : m_lawnMowers) {
                if (mower.getRow() == zRow && !mower.isFinished()) {
                    rowHasMower = true;
                    break;
                }
            }
            if (!rowHasMower) {
                m_levelLost = true;
            }
        }
    }

    // 3. LawnMower Clears
    for (auto& mower : m_lawnMowers) {
        if (!mower.isTriggered() || mower.isFinished()) continue;
        int mRow = mower.getRow();
        float mX = mower.getX();

        for (int c = 0; c < 9; ++c) {
            float cellX = 140.0f + (c == 0 ? 0.0f : 80.0f + (c - 1) * 70.0f);
            if (mX >= cellX && m_grid[mRow][c] != nullptr) {
                m_grid[mRow][c] = nullptr;
            }
        }

        for (auto& z : m_zombies) {
            if (z->isDead() || z->isDevoured()) continue;
            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
            if (zRow == mRow) {
                if (std::abs(z->getX() - mX) <= 50.0f || (z->getX() <= mX && z->getX() >= mX - 60.0f)) {
                    z->takeDamage(1800.0f);
                }
            }
        }
    }
}

void Level4::update(float dt) {
    if (m_inGameMenu && m_inGameMenu->isOpen()) {
        InGameMenuAction action = m_inGameMenu->update(dt);
        if (action == InGameMenuAction::RestartLevel) {
            restartLevel();
        } else if (action == InGameMenuAction::MainMenu) {
            m_exitToMainMenu = true;
        }
        return;
    }

    if (!m_levelWon && !m_levelLost && IsKeyPressed(KEY_ESCAPE)) {
        if (m_inGameMenu) m_inGameMenu->open();
        return;
    }

    Vector2 mousePos = GetVirtualMousePosition();
    bool mouseClicked = false;
    if (m_ignoreInitialClick) {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            m_ignoreInitialClick = false;
        }
    } else {
        mouseClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    if (mouseClicked && CheckCollisionPointRec(mousePos, menuBtnRect)) {
        if (m_inGameMenu) m_inGameMenu->open();
        return;
    }

    if (m_phase == LevelPhase::SeedSelection) {
        for (auto& item : m_previewZombies) {
            item.zombie->getAnim().Update(dt);
        }
        bool startPan = m_seedSelectMenu.update(dt, mousePos, mouseClicked);
        if (startPan) {
            m_phase = LevelPhase::PanToLawn;
            m_panTimer = 0.0f;
            m_seedBank.initFromDeck(m_seedSelectMenu.getChosenDeck());
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/gravebutton.ogg"));
        }
        return;
    }

    if (m_phase == LevelPhase::PanToLawn) {
        for (auto& item : m_previewZombies) {
            item.zombie->getAnim().Update(dt);
        }
        m_panTimer += dt;
        float t = std::min(1.0f, m_panTimer / m_panDuration);
        float easeT = t * t * (3.0f - 2.0f * t);
        m_cameraCropX = 500.0f + (90.0f - 500.0f) * easeT;

        if (t >= 1.0f) {
            m_cameraCropX = 90.0f;
            m_phase = LevelPhase::ReadySetPlant;
            m_previewZombies.clear();
            m_readySetPlantTimer = 0.0f;
            m_readySetPlantAnim.SetFrame(0.0f);
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/readysetplant.ogg"));
        }
        return;
    }

    if (m_phase == LevelPhase::ReadySetPlant) {
        m_readySetPlantTimer += dt;
        m_readySetPlantAnim.Update(dt);

        if (!m_introGraveSoundPlayed) {
            m_introGraveSoundPlayed = true;
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/dirt_rise.ogg"));
        }

        // Graves rising up from underground during Ready Set Plant
        for (auto& g : m_graves) {
            g.introRiseTimer += dt * 0.9f;
            float gx = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f) - 6.0f;
            float gy = 80.0f + g.row * 100.0f + 6.0f;
            if (g.introRiseTimer < 1.0f && GetRandomValue(0, 2) == 0) {
                createGraveDirtParticle(gx + 43.0f, gy + 75.0f, 3);
            }
        }

        if (m_readySetPlantTimer >= 1.9f) {
            for (auto& g : m_graves) g.introRiseTimer = 1.0f;
            m_phase = LevelPhase::ActiveWave;
            AudioManager::GetInstance().PlayMusic(MusicTrack::NightLevel);
        }
        return;
    }

    Rectangle pauseBtn = { 668.0f, 538.0f, 26.0f, 26.0f };
    Rectangle speedBtn = { 698.0f, 538.0f, 85.0f, 26.0f };

    if (!m_levelWon && !m_levelLost) {
        if (IsKeyPressed(KEY_SPACE) || (mouseClicked && CheckCollisionPointRec(mousePos, pauseBtn))) {
            m_isSpeedPaused = !m_isSpeedPaused;
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/pause.ogg"));
            return;
        }

        if (IsKeyPressed(KEY_F) || (mouseClicked && CheckCollisionPointRec(mousePos, speedBtn))) {
            if (m_gameSpeed == 1.0f) m_gameSpeed = 2.0f;
            else if (m_gameSpeed == 2.0f) m_gameSpeed = 4.0f;
            else m_gameSpeed = 1.0f;
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buttonclick.ogg"));
            return;
        }
    }

    if (m_levelWon) {
        m_winTimer += dt;
        m_awardRaysRotation += 40.0f * dt;
        m_awardY = std::min(240.0f, m_awardY + (240.0f - m_awardY) * 6.0f * dt + 200.0f * dt);

        if (!m_winMusicPlayed) {
            m_winMusicPlayed = true;
            ProfileManager::GetInstance().UnlockNextLevel(m_levelNumber);
            ProfileManager::GetInstance().AddCoins(500);
            AudioManager::GetInstance().PlayMusic(MusicTrack::None);
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/winmusic.ogg"));
        }

        if (m_winTimer >= 1.5f && (mouseClicked || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
            m_exitToMainMenu = true;
        }
        return;
    }

    if (m_levelLost) {
        m_loseTimer += dt;

        if (!m_screamSoundPlayed) {
            m_screamSoundPlayed = true;
            AudioManager::GetInstance().PlayMusic(MusicTrack::None);
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/scream.ogg"));
        }

        if (m_loseTimer >= 0.7f && !m_loseMusicPlayed) {
            m_loseMusicPlayed = true;
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/losemusic.ogg"));
        }

        if (m_loseTimer >= 2.0f && (mouseClicked || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE))) {
            restartLevel();
        }
        return;
    }

    float simDt = m_isSpeedPaused ? 0.0f : dt * m_gameSpeed;
    m_fogTimer += simDt;

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c] && m_grid[r][c]->isDead()) {
                m_grid[r][c] = nullptr;
            } else if (m_grid[r][c]) {
                bool shoot = false;
                float plantX = (float)m_grid[r][c]->getX();
                float min_distance = 800.0f;
                for (auto &zombie : m_zombies) {
                    if (!zombie->isDead() && !zombie->isDevoured()) {
                        int zRow = (int)((zombie->getY() - 45.0f + 50.0f) / 100.0f);
                        if (zRow == r) {
                            float dist = zombie->getX() - plantX;
                            if (dist >= 0.0f && dist <= 750.0f) {
                                shoot = true;
                                min_distance = std::min(min_distance, dist);
                                break;
                            }
                        }
                    }
                }

                std::string plantName = m_grid[r][c]->getName();
                if (plantName == "CherryBomb" || plantName == "Jalapeno" || plantName == "PotatoMine" || plantName == "Squash" || plantName == "IceShroom" || plantName == "Chomper" || plantName == "Caltrop" || plantName == "SpikeRock" || plantName == "Gravebuster") {
                    continue;
                }

                std::string targetAnim;
                if (shoot) {
                    targetAnim = (plantName == "SunFlower" || plantName == "Wallnut" ||
                                  plantName == "Garlic" || plantName == "Gravebuster" ||
                                  plantName == "Caltrop" || plantName == "SpikeRock" ||
                                  plantName == "Chomper") ? "anim_idle" : "anim_shooting";
                } else {
                    targetAnim = (plantName == "SunFlower" || plantName == "Wallnut" ||
                                  plantName == "Garlic" || plantName == "Gravebuster" ||
                                  plantName == "Caltrop" || plantName == "SpikeRock" ||
                                  plantName == "Chomper" || plantName == "Melonpult" ||
                                  plantName == "Cornpult" || plantName == "Cabbagepult") ? "anim_idle" : "anim_head_idle";
                }

                if (shoot && (plantName == "Melonpult" || plantName == "Cornpult" || plantName == "Cabbagepult")) {
                    m_grid[r][c]->set_distance(min_distance - 65.0f);
                }

                if (m_grid[r][c]->getAnim().GetCurrentAnimName() != targetAnim) {
                    m_grid[r][c]->SetAnimation(targetAnim);
                }
            }
        }
    }

    m_seedBank.update(simDt, mousePos, mouseClicked);

    // Plant placement logic
    int hoverRow, hoverCol;
    if (mouseClicked && getGridCell(mousePos, hoverRow, hoverCol)) {
        if (m_seedBank.isShovelSelected()) {
            if (m_grid[hoverRow][hoverCol]) {
                m_grid[hoverRow][hoverCol] = nullptr;
                m_seedBank.deselect();
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/plant2.ogg"));
            }
        } else {
            std::string selectedType = m_seedBank.getSelectedPlantType();
            if (!selectedType.empty()) {
                if (m_grid[hoverRow][hoverCol] == nullptr) {
                    bool isGrave = isCellBlockedByGrave(hoverRow, hoverCol);
                    bool canPlant = (selectedType == "Gravebuster") ? isGrave : !isGrave;

                    if (canPlant) {
                        float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
                        float cellH = 100.0f;
                        float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
                        float cellY = 80.0f + hoverRow * 100.0f;
                        
                        int px, py;
                        if (selectedType == "Gravebuster") {
                            float gx = cellX - 6.0f;
                            float gy = cellY + 6.0f;
                            px = (int)(gx - 2.0f);
                            py = (int)(gy - 28.0f); // Starts higher up above the grave
                        } else {
                            float centerX = cellX + cellW / 2.0f;
                            float centerY = cellY + cellH / 2.0f;
                            px = (int)(centerX - 30.0f - 10.0f);
                            py = (int)(centerY - 35.0f);
                        }
                        createPlant(selectedType, hoverRow, hoverCol, px, py);
                        m_seedBank.consumeSelected();

                        int plantSfxChoice = GetRandomValue(0, 1);
                        std::string plantSfx = (plantSfxChoice == 0) ? "assets/sounds/plant.ogg" : "assets/sounds/plant2.ogg";
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(plantSfx));
                    } else {
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
                    }
                } else {
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
                }
            }
        }
    }

    // Sun Click Collection
    if (mouseClicked) {
        for (auto& s : m_suns) {
            if (s.isActive() && s.isClicked(mousePos)) {
                s.collect(20.0f, 15.0f);
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/points.ogg"));
            }
        }
    }

    // Sub-stepping simulation loop
    float remainingDt = simDt;
    const float maxSubStep = 0.0166f;

    while (remainingDt > 0.0f) {
        float subDt = std::min(remainingDt, maxSubStep);
        remainingDt -= subDt;

        if (m_currentWave < m_maxWaves) {
            m_waveTimer -= subDt;
            if (m_waveTimer <= 0.0f) {
                spawnNextWave();
                m_waveTimer = 22.0f;
            }
        }

        // Update shaking graves & trigger rising zombies + grave destruction
        for (auto& g : m_graves) {
            if (g.isDestroyed || g.shakeTimer <= 0.0f) continue;
            g.shakeTimer -= subDt;

            float cellX = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f);
            float cellY = 80.0f + g.row * 100.0f;

            if (GetRandomValue(0, 2) == 0) {
                createGraveDirtParticle(cellX + 35.0f, cellY + 75.0f, 4);
            }

            if (g.shakeTimer <= 0.0f) {
                // Grave breaks and disappears!
                g.isDestroyed = true;
                createGraveCrumbleParticles(cellX + 35.0f, cellY + 75.0f);
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/dirt_rise.ogg"));

                float gx = cellX - 5.0f;
                float gy = 45.0f + g.row * 100.0f;

                std::unique_ptr<Zombie> newZ;
                if (g.pendingZombieType == "BucketheadZombie") {
                    newZ = std::make_unique<BucketheadZombie>(res, gx, gy + 70.0f);
                } else if (g.pendingZombieType == "ConeheadZombie") {
                    newZ = std::make_unique<ConeheadZombie>(res, gx, gy + 70.0f);
                } else {
                    newZ = std::make_unique<ZombieNormal>(res, gx, gy + 70.0f);
                }

                if (newZ) {
                    newZ->getAnim().SetBaseAnimation("anim_walk");
                    newZ->getAnim().SetAnimation("anim_walk");
                    m_risingZombies.push_back({ std::move(newZ), gy, 70.0f, 0.0f, 0.9f, g.row });
                }
                g.pendingZombieType = "";
            }
        }

        // Update Rising Zombies
        for (auto it = m_risingZombies.begin(); it != m_risingZombies.end(); ) {
            it->riseTimer += subDt;
            float t = std::min(1.0f, it->riseTimer / it->maxRiseTime);
            it->currentYOffset = 70.0f * (1.0f - t);
            it->zombie->setY(it->targetY + it->currentYOffset);
            it->zombie->getAnim().Update(subDt);

            if (t >= 1.0f) {
                it->zombie->setY(it->targetY);
                m_zombies.push_back(std::move(it->zombie));
                it = m_risingZombies.erase(it);
            } else {
                ++it;
            }
        }

        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                    m_grid[r][c]->update(subDt, m_projectiles, m_suns);

                    if (m_grid[r][c]->getName() == "Gravebuster") {
                        Gravebuster* gb = dynamic_cast<Gravebuster*>(m_grid[r][c].get());
                        if (gb) {
                            for (auto& g : m_graves) {
                                if (!g.isDestroyed && g.row == r && g.col == c) {
                                    if (gb->isFinished() || gb->getEatProgress() >= 1.0f) {
                                        g.isDestroyed = true;
                                        float cellX = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f);
                                        float cellY = 80.0f + g.row * 100.0f;
                                        createGraveCrumbleParticles(cellX + 35.0f, cellY + 75.0f);
                                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/dirt_rise.ogg"));
                                        Texture2D texSun = res.GetTexture("SUN");
                                        if (texSun.id == 0) texSun = res.GetTexture("Sun");
                                        m_suns.emplace_back(cellX + 25.0f, cellY + 15.0f, texSun);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (auto& z : m_zombies) {
            if (!z->isFinished()) {
                z->update(subDt);
            }
        }

        for (auto& p : m_projectiles) {
            p.update(subDt);
        }

        for (auto& mower : m_lawnMowers) {
            mower.update(subDt);
        }

        for (auto& s : m_suns) {
            s.update(subDt);
            if (s.hasArrived()) {
                m_seedBank.addSun(25);
            }
        }

        for (auto& eff : m_effects) {
            if (!eff.active) continue;
            eff.timing(subDt);
        }

        updateCollisions(subDt);
        updateFog(subDt);
    }

    // Despawn Dead Zombies & Inactive Projectiles
    m_zombies.erase(
        std::remove_if(m_zombies.begin(), m_zombies.end(),
            [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }),
        m_zombies.end()
    );

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }),
        m_projectiles.end()
    );

    m_suns.erase(
        std::remove_if(m_suns.begin(), m_suns.end(),
            [](SunItem& s) { return !s.isActive() || s.hasArrived(); }),
        m_suns.end()
    );

    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(),
            [](const ParticleEffect& eff) { return !eff.active; }),
        m_effects.end()
    );

    if (m_currentWave >= m_maxWaves && m_zombies.empty() && m_risingZombies.empty() && !m_levelWon && !m_levelLost) {
        m_levelWon = true;
    }
}

void Level4::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Night Front Lawn Background (background2.jpg)
    if (m_texBgNight.id != 0) {
        DrawTexturePro(
            m_texBgNight,
            { m_cameraCropX, 0.0f, 900.0f, 600.0f },
            { 0.0f, 0.0f, 800.0f, 600.0f },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangle(0, 0, 800, 600, DARKBLUE);
    }

    Vector2 mousePos = GetVirtualMousePosition();

    if (m_phase == LevelPhase::SeedSelection) {
        for (auto& item : m_previewZombies) {
            float screenX = (item.worldX - m_cameraCropX) * (800.0f / 900.0f);
            item.zombie->setX(screenX);
            item.zombie->setY(item.worldY);
            item.zombie->draw();
        }
        m_seedSelectMenu.draw(res, mousePos);
    } else if (m_phase == LevelPhase::PanToLawn) {
        for (auto& item : m_previewZombies) {
            float screenX = (item.worldX - m_cameraCropX) * (800.0f / 900.0f);
            item.zombie->setX(screenX);
            item.zombie->setY(item.worldY);
            item.zombie->draw();
        }
        m_seedBank.draw(res, mousePos);
    } else if (m_phase == LevelPhase::ReadySetPlant) {
        // Draw Row-by-Row graves rising from underground
        for (int r = 0; r < 5; ++r) {
            for (const auto& g : m_graves) {
                if (g.row == r && !g.isDestroyed) {
                    float gx = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f) - 6.0f;
                    float gy = 80.0f + g.row * 100.0f + 6.0f;
                    float introYOffset = (1.0f - std::min(1.0f, g.introRiseTimer)) * 80.0f;

                    if (m_texTombstones.id != 0) {
                        Rectangle srcTomb = { (float)g.frameCol * 86.0f, (float)g.frameRow * 91.0f, 86.0f, 91.0f };
                        DrawTexturePro(m_texTombstones, srcTomb, { gx, gy + introYOffset, 86.0f, 91.0f }, { 0, 0 }, 0.0f, WHITE);
                    }

                    if (m_texMounds.id != 0) {
                        Rectangle srcMound = { (float)g.frameCol * 86.0f, (float)g.frameRow * 91.0f, 86.0f, 91.0f };
                        DrawTexturePro(m_texMounds, srcMound, { gx, gy + introYOffset, 86.0f, 91.0f }, { 0, 0 }, 0.0f, WHITE);
                    }
                }
            }
        }
        for (const auto& eff : m_effects) {
            eff.draw();
        }
        m_seedBank.draw(res, mousePos);
        m_readySetPlantAnim.Draw(400.0f, 300.0f, 1.0f);
    } else {
        // Grid hover highlight
        int hoverRow, hoverCol;
        if (getGridCell(mousePos, hoverRow, hoverCol)) {
            float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
            float cellY = 80.0f + hoverRow * 100.0f;
            float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
            float cellH = 100.0f;
            bool isGrave = isCellBlockedByGrave(hoverRow, hoverCol);
            std::string selType = m_seedBank.getSelectedPlantType();
            bool isValid = (selType == "Gravebuster") ? (isGrave && m_grid[hoverRow][hoverCol] == nullptr) : (!isGrave && m_grid[hoverRow][hoverCol] == nullptr);
            DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, isValid ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(RED, 0.6f));
        }

        // Draw Row-by-Row from Top (Row 0) to Bottom (Row 4) for proper depth
        for (int r = 0; r < 5; ++r) {
            // A. LawnMower
            for (const auto& mower : m_lawnMowers) {
                if (mower.getRow() == r) {
                    mower.draw();
                }
            }

            // B. Tombstone Slabs in Row r (with horizontal rumble shake animation, cracking, and Gravebuster sinking)
            for (const auto& g : m_graves) {
                if (g.row == r && !g.isDestroyed) {
                    float gx = 140.0f + (g.col == 0 ? 0.0f : 80.0f + (g.col - 1) * 70.0f) - 6.0f;
                    float gy = 80.0f + g.row * 100.0f + 6.0f;

                    float shakeOffset = (g.shakeTimer > 0.0f) ? (sinf(g.shakeTimer * 60.0f) * 3.5f) : 0.0f;
                    
                    float gbProgress = 0.0f;
                    Plant* p = m_grid[g.row][g.col].get();
                    if (p && p->getName() == "Gravebuster") {
                        Gravebuster* gb = dynamic_cast<Gravebuster*>(p);
                        if (gb) gbProgress = gb->getEatProgress();
                    }

                    int drawRow = g.frameRow;
                    if (gbProgress > 0.66f) {
                        drawRow = 3;
                    } else if (gbProgress > 0.33f) {
                        drawRow = 2;
                    } else if (gbProgress > 0.0f) {
                        drawRow = 1;
                    } else if (g.shakeTimer > 0.0f) {
                        drawRow = (g.shakeTimer < 0.4f) ? 2 : 1;
                    }

                    if (m_texTombstones.id != 0) {
                        Rectangle srcTomb = { (float)g.frameCol * 86.0f, (float)drawRow * 91.0f, 86.0f, 91.0f };
                        DrawTexturePro(m_texTombstones, srcTomb, { gx + shakeOffset, gy, 86.0f, 91.0f }, { 0, 0 }, 0.0f, WHITE);
                    }

                    if (m_texMounds.id != 0) {
                        Rectangle srcMound = { (float)g.frameCol * 86.0f, (float)g.frameRow * 91.0f, 86.0f, 91.0f };
                        DrawTexturePro(m_texMounds, srcMound, { gx + shakeOffset, gy, 86.0f, 91.0f }, { 0, 0 }, 0.0f, WHITE);
                    }
                }
            }

            // C. Rising Zombies in Row r
            for (const auto& rz : m_risingZombies) {
                if (rz.row == r) {
                    rz.zombie->draw();
                }
            }

            // D. Plants in Row r (Gravebuster drawn OVER the grave slab and mound base!)
            for (int c = 0; c < 9; ++c) {
                if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                    m_grid[r][c]->draw();
                }
            }

            // E. Active Walking Zombies in Row r
            for (const auto& z : m_zombies) {
                if (!z->isFinished()) {
                    int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                    if (zRow == r) {
                        z->draw();
                    }
                }
            }
        }

        // Draw Fog layer over rightmost columns (if fog level)
        if (m_hasFog) {
            drawFog();
        }

        // Draw Top SeedBank & FlagMeter UI
        m_seedBank.draw(res, mousePos);
        drawProgressBar();
        drawSpeedControls();

        // Projectiles (rendered above SeedBank for high lobbed arcs!), particles, suns
        for (const auto& p : m_projectiles) {
            p.draw();
        }
        for (const auto& eff : m_effects) {
            eff.draw();
        }
        for (const auto& s : m_suns) {
            s.draw();
        }

        if (m_levelWon) {
            drawWinScreen();
        } else if (m_levelLost) {
            drawLoseScreen();
        }
    }

    // In-game pause menu button & dialog
    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    bool menuHovered = CheckCollisionPointRec(mousePos, menuBtnRect);
    bool menuPressed = menuHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (m_inGameMenu) {
        m_inGameMenu->drawMenuButton(menuHovered, menuPressed);
    }
    if (m_inGameMenu && m_inGameMenu->isOpen()) {
        m_inGameMenu->draw();
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(
        targetScreen.texture,
        { 0.0f, 0.0f, (float)targetScreen.texture.width, -(float)targetScreen.texture.height },
        { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
    EndDrawing();
}

void Level4::drawProgressBar() {
    Texture2D texMeter = res.GetTexture("FLAGMETER");
    Texture2D texBadge = res.GetTexture("FLAGMETERLEVELPROGRESS");
    Texture2D texParts = res.GetTexture("FLAGMETERPARTS");

    if (texMeter.id == 0) return;

    float waveProgress = (float)m_currentWave / (float)std::max(1, m_maxWaves);
    if (m_currentWave < m_maxWaves) {
        float waveFraction = std::clamp((22.0f - m_waveTimer) / 22.0f, 0.0f, 1.0f);
        waveProgress = ((float)m_currentWave + waveFraction) / (float)m_maxWaves;
    }
    waveProgress = std::clamp(waveProgress, 0.0f, 1.0f);

    float barX = 625.0f;
    float barY = 572.0f;

    // 1. Level text: "Level 4", "Level 5", "Level 6"
    std::string levelStr = "Level " + std::to_string(m_levelNumber);
    float fontScale = 0.65f;
    int textW = m_font.MeasureText(levelStr.c_str(), fontScale);
    float textX = barX - (float)textW - 10.0f;
    float textY = 566.0f;

    // Drop shadow
    m_font.DrawText(levelStr.c_str(), textX + 2.0f, textY + 2.0f, fontScale, Color{ 0, 0, 0, 255 });
    // Golden yellow text
    m_font.DrawText(levelStr.c_str(), textX, textY, fontScale, Color{ 235, 200, 45, 255 });

    // 2. Progress Bar Frame
    DrawTextureRec(texMeter, { 0.0f, 0.0f, 158.0f, 25.0f }, { barX, barY }, WHITE);

    // 3. Green Progress Fill
    float maxFillWidth = 149.0f;
    float currentFillWidth = maxFillWidth * waveProgress;
    if (currentFillWidth > 0.0f) {
        DrawTextureRec(texMeter, { 155.0f - currentFillWidth, 27.0f, currentFillWidth, 24.0f }, { barX + 155.0f - currentFillWidth, barY }, WHITE);
    }

    // 4. "LEVEL PROGRESS" badge
    if (texBadge.id != 0) {
        DrawTextureRec(texBadge, { 0.0f, 0.0f, (float)texBadge.width, (float)texBadge.height }, { barX + 36.0f, barY + 13.0f }, WHITE);
    }

    // 5. Red Flags at wave 5 and wave 8
    if (texParts.id != 0) {
        std::vector<float> flagFractions = { 5.0f / 8.0f, 1.0f };
        for (float frac : flagFractions) {
            float flagX = barX + 155.0f - maxFillWidth * frac - 8.0f;
            flagX = std::max(barX + 6.0f, flagX);

            DrawTextureRec(texParts, { 25.0f, 0.0f, 25.0f, 25.0f }, { flagX, barY - 2.0f }, WHITE);

            float flagOffsetY = (waveProgress >= frac - 0.05f) ? -6.0f : -2.0f;
            DrawTextureRec(texParts, { 50.0f, 0.0f, 25.0f, 25.0f }, { flagX, barY + flagOffsetY }, WHITE);
        }

        // 6. Zombie Head Slider Marker
        float headX = barX + 155.0f - currentFillWidth - 11.0f;
        headX = std::clamp(headX, barX + 6.0f, barX + 144.0f);
        DrawTextureRec(texParts, { 0.0f, 0.0f, 25.0f, 25.0f }, { headX, barY - 2.0f }, WHITE);
    }
}

void Level4::drawSpeedControls() {
    Vector2 mousePos = GetVirtualMousePosition();
    Rectangle pauseBtn = { 668.0f, 538.0f, 26.0f, 26.0f };
    Rectangle speedBtn = { 698.0f, 538.0f, 85.0f, 26.0f };

    bool pauseHover = CheckCollisionPointRec(mousePos, pauseBtn);
    bool speedHover = CheckCollisionPointRec(mousePos, speedBtn);

    DrawRectangleRounded(pauseBtn, 0.25f, 4, pauseHover ? Color{ 85, 95, 135, 255 } : Color{ 60, 68, 105, 255 });
    DrawRectangleRoundedLines(pauseBtn, 0.25f, 4, 1.5f, pauseHover ? Color{ 140, 160, 220, 255 } : Color{ 100, 115, 165, 255 });

    if (m_isSpeedPaused) {
        DrawTriangle({ 677, 544 }, { 677, 558 }, { 688, 551 }, GREEN);
    } else {
        DrawRectangle(675, 544, 4, 14, RAYWHITE);
        DrawRectangle(683, 544, 4, 14, RAYWHITE);
    }

    DrawRectangleRounded(speedBtn, 0.25f, 4, speedHover ? Color{ 85, 95, 135, 255 } : Color{ 60, 68, 105, 255 });
    DrawRectangleRoundedLines(speedBtn, 0.25f, 4, 1.5f, speedHover ? Color{ 140, 160, 220, 255 } : Color{ 100, 115, 165, 255 });
    DrawTriangle({ speedBtn.x + 8.0f, speedBtn.y + 7.0f },
                 { speedBtn.x + 8.0f, speedBtn.y + speedBtn.height - 7.0f },
                 { speedBtn.x + 19.0f, speedBtn.y + speedBtn.height / 2.0f },
                 Color{ 220, 235, 255, 255 });

    char speedText[16];
    snprintf(speedText, sizeof(speedText), "%.0fx", m_gameSpeed);
    DrawText(speedText, (int)(speedBtn.x + 24.0f), (int)(speedBtn.y + 4.0f), 17, (m_gameSpeed > 1.0f) ? Color{ 255, 220, 40, 255 } : Color{ 230, 235, 245, 255 });
}

void Level4::drawWinScreen() {
    float overlayAlpha = std::clamp(m_winTimer * 1.5f, 0.0f, 0.75f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(BLACK, overlayAlpha));

    Texture2D texRays = res.GetTexture("AWARDRAYS");
    if (texRays.id != 0) {
        float rayScale = std::min(1.0f, m_winTimer * 1.2f);
        float rayW = (float)texRays.width * rayScale;
        float rayH = (float)texRays.height * rayScale;
        DrawTexturePro(
            texRays,
            { 0.0f, 0.0f, (float)texRays.width, (float)texRays.height },
            { 400.0f, 250.0f, rayW, rayH },
            { rayW / 2.0f, rayH / 2.0f },
            m_awardRaysRotation,
            ColorAlpha(WHITE, std::min(1.0f, m_winTimer * 2.0f))
        );
    }

    Texture2D texTrophy = res.GetTexture("TROPHY_HI_RES");
    if (texTrophy.id == 0) texTrophy = res.GetTexture("Trophy_hi_res");

    if (texTrophy.id != 0) {
        DrawTexturePro(
            texTrophy,
            { 0.0f, 0.0f, (float)texTrophy.width, (float)texTrophy.height },
            { 400.0f, m_awardY, (float)texTrophy.width, (float)texTrophy.height },
            { (float)texTrophy.width / 2.0f, (float)texTrophy.height / 2.0f },
            0.0f,
            WHITE
        );
    }

    if (m_winTimer >= 1.2f) {
        float textAlpha = std::clamp((m_winTimer - 1.2f) * 2.5f, 0.0f, 1.0f);
        DrawRectangleRec({ 220, 390, 360, 95 }, ColorAlpha(BLACK, textAlpha * 0.8f));
        DrawRectangleLinesEx({ 220, 390, 360, 95 }, 2.0f, ColorAlpha(GOLD, textAlpha));
        DrawText("NIGHT STAGE COMPLETED!", 245, 405, 22, ColorAlpha(GOLD, textAlpha));
        DrawText("Click anywhere to continue", 280, 445, 17, ColorAlpha(RAYWHITE, textAlpha));
    }
}

void Level4::drawLoseScreen() {
    float overlayAlpha = std::clamp(m_loseTimer * 1.8f, 0.0f, 0.88f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(Color{ 25, 0, 0, 255 }, overlayAlpha));

    Texture2D texZombiesWon = res.GetTexture("ZOMBIESWON");
    if (texZombiesWon.id != 0) {
        float zoomProgress = std::clamp((m_loseTimer - 0.4f) * 1.8f, 0.0f, 1.0f);
        float easeZoom = zoomProgress * zoomProgress * (3.0f - 2.0f * zoomProgress);
        float scale = 0.5f + 0.5f * easeZoom;
        float w = (float)texZombiesWon.width * scale;
        float h = (float)texZombiesWon.height * scale;

        DrawTexturePro(
            texZombiesWon,
            { 0.0f, 0.0f, (float)texZombiesWon.width, (float)texZombiesWon.height },
            { 400.0f, 250.0f, w, h },
            { w / 2.0f, h / 2.0f },
            0.0f,
            ColorAlpha(WHITE, std::min(1.0f, (m_loseTimer - 0.3f) * 2.5f))
        );
    } else {
        DrawText("THE ZOMBIES ATE YOUR BRAINS!", 190, 220, 26, RED);
    }

    if (m_loseTimer >= 1.6f) {
        float textAlpha = std::clamp((m_loseTimer - 1.6f) * 2.0f, 0.0f, 1.0f);
        DrawRectangleRec({ 245, 470, 310, 44 }, ColorAlpha(BLACK, textAlpha * 0.75f));
        DrawRectangleLinesEx({ 245, 470, 310, 44 }, 2.0f, ColorAlpha(RED, textAlpha));
        DrawText("Click anywhere to try again", 270, 483, 18, ColorAlpha(RAYWHITE, textAlpha));
    }
}

void Level4::updateFog(float dt) {
    if (!m_hasFog) return;

    m_fogTimer += dt;

    // 1. Reset target bounds to the level baseline fog start
    for (int r = 0; r < 5; ++r) {
        m_targetRowFogStartX[r] = m_baseFogStartX;
    }

    // 2. Scan all active Planterns and push fog to the right
    // Plantern at (r, c) clears fog on row r-1, r, r+1 up to the right edge of column (c + 3)
    // Lawn starts at 140px, column width is 70px.
    // Right boundary of column (c + 3) is 140.0f + (c + 4) * 70.0f
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            Plant* p = m_grid[r][c].get();
            if (p && p->getName() == "Plantern" && !p->isDead()) {
                float clearRightX = 140.0f + (float)(c + 4) * 70.0f;
                for (int dr = -1; dr <= 1; ++dr) {
                    int targetRow = r + dr;
                    if (targetRow >= 0 && targetRow < 5) {
                        m_targetRowFogStartX[targetRow] = std::max(m_targetRowFogStartX[targetRow], clearRightX);
                    }
                }
            }
        }
    }

    // 3. Smoothly slide current fog horizontal boundaries toward target boundaries
    float fogSpeed = 350.0f; // px/sec for smooth horizontal roll
    for (int r = 0; r < 5; ++r) {
        if (m_rowFogStartX[r] < m_targetRowFogStartX[r]) {
            m_rowFogStartX[r] = std::min(m_targetRowFogStartX[r], m_rowFogStartX[r] + fogSpeed * dt);
        } else if (m_rowFogStartX[r] > m_targetRowFogStartX[r]) {
            m_rowFogStartX[r] = std::max(m_targetRowFogStartX[r], m_rowFogStartX[r] - fogSpeed * dt);
        }
    }
}

void Level4::drawFog() {
    if (!m_hasFog) return;

    struct FogBand {
        float y;
        float h;
        int rowIdx;
    };
    FogBand bands[7] = {
        { -50.0f, 130.0f, 0 }, // Top border up to row 0 top (y: -50 to 80)
        { 80.0f,  100.0f, 0 }, // Row 0 (y: 80 to 180)
        { 180.0f, 100.0f, 1 }, // Row 1 (y: 180 to 280)
        { 280.0f, 100.0f, 2 }, // Row 2 (y: 280 to 380)
        { 380.0f, 100.0f, 3 }, // Row 3 (y: 380 to 480)
        { 480.0f, 100.0f, 4 }, // Row 4 (y: 480 to 580)
        { 580.0f, 120.0f, 4 }  // Bottom border (y: 580 to 700)
    };

    // 1. Solid opaque atmospheric darkness & fog backing per row slice
    for (int b = 0; b < 7; ++b) {
        float startX = m_rowFogStartX[bands[b].rowIdx];
        DrawRectangleGradientH(
            (int)(startX - 30.0f), (int)bands[b].y,
            60, (int)bands[b].h,
            Color{ 16, 20, 36, 0 }, Color{ 16, 20, 36, 255 }
        );
        DrawRectangle(
            (int)(startX + 30.0f), (int)bands[b].y,
            (int)(900.0f - (startX + 30.0f)), (int)bands[b].h,
            Color{ 16, 20, 36, 255 }
        );
    }

    // 2. Rolling PopCap Fog Cloud Textures per row slice
    Texture2D texSoft = res.GetTexture("FOG_SOFTWARE");
    if (texSoft.id == 0) texSoft = res.GetTexture("fog_software");

    if (texSoft.id != 0) {
        for (int b = 0; b < 7; ++b) {
            float startX = m_rowFogStartX[bands[b].rowIdx];
            float rowY = bands[b].y - 25.0f;
            float driftY = cosf(m_fogTimer * 0.9f + (float)b * 1.1f) * 8.0f;

            for (float colX = startX - 25.0f; colX <= 850.0f; colX += 115.0f) {
                int cloudIdx = ((int)((colX + (float)b * 70.0f) / 115.0f) % 3 + 3) % 3;
                float driftX = sinf(m_fogTimer * 0.7f + (float)b * 1.4f + colX * 0.05f) * 14.0f;

                Rectangle srcCloud = { (float)cloudIdx * 189.0f, 0.0f, 189.0f, 144.0f };
                Rectangle destCloud = { colX + driftX, rowY + driftY, 215.0f, 155.0f };

                DrawTexturePro(texSoft, srcCloud, destCloud, { 0, 0 }, 0.0f, Color{ 230, 238, 255, 255 });
            }
        }
    } else if (m_texFog.id != 0) {
        for (int b = 0; b < 7; ++b) {
            float startX = m_rowFogStartX[bands[b].rowIdx];
            float rowY = bands[b].y - 20.0f;
            float driftX = sinf(m_fogTimer * 0.8f + (float)b * 1.3f) * 16.0f;
            float driftY = cosf(m_fogTimer * 1.1f + (float)b * 0.9f) * 6.0f;

            for (float colX = startX - 30.0f; colX <= 850.0f; colX += 200.0f) {
                Rectangle srcRect = { 0.0f, 0.0f, (float)m_texFog.width * 0.35f, (float)m_texFog.height };
                Rectangle destRect = { colX + driftX, rowY + driftY, 260.0f, 160.0f };
                DrawTexturePro(m_texFog, srcRect, destRect, { 0, 0 }, 0.0f, Color{ 220, 230, 250, 255 });
            }
        }
    }

    // 3. Plantern Fog Illumination & Piercing Light
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            Plant* p = m_grid[r][c].get();
            if (p && p->getName() == "Plantern" && !p->isDead()) {
                int cx = p->getX() + 38;
                int cy = p->getY() + 45;
                // Warm ambient light piercing the fog
                DrawCircleGradient(cx, cy, 180.0f, ColorAlpha(GOLD, 0.50f), ColorAlpha(GOLD, 0.0f));
                DrawCircleGradient(cx, cy, 110.0f, ColorAlpha(YELLOW, 0.65f), ColorAlpha(GOLD, 0.0f));
                DrawCircleGradient(cx, cy, 55.0f, ColorAlpha(WHITE, 0.85f), ColorAlpha(YELLOW, 0.0f));
                // Redraw Plantern entity on top of the illuminated fog clearing
                p->draw();
            }
        }
    }
}

void Level4::run() {
    SetUIInteractionEnabled(true);
    AudioManager::GetInstance().PlayMusic(MusicTrack::None);
    while (!WindowShouldClose()) {
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        float dt = GetFrameTime();
        AudioManager::GetInstance().Update();
        update(dt);
        draw();

        if (m_exitToMainMenu || (m_levelWon && IsKeyPressed(KEY_ENTER)) || ((m_levelWon || m_levelLost) && IsKeyPressed(KEY_ESCAPE))) {
            break;
        }
    }
}
