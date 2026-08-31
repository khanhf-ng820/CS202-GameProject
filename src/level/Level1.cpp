#include "Level1.h"
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
#include "TwinSunflower.h"
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

Level1::Level1(Resources& res, RenderTexture2D targetScreen, int levelNumber)
    : res(res), targetScreen(targetScreen), m_levelNumber(levelNumber),
      m_phase(LevelPhase::SeedSelection),
      m_seedSelectMenu(res), m_seedBank(40000),
      m_skySunTimer(0.0f), m_waveTimer(14.0f), m_currentWave(0),
      m_maxWaves(5), m_levelWon(false), m_levelLost(false),
      m_finalWaveAnnounced(false), m_exitToMainMenu(false),
      m_gameSpeed(1.0f), m_isSpeedPaused(false),
      m_winTimer(0.0f), m_awardY(-150.0f), m_awardRaysRotation(0.0f), m_winMusicPlayed(false),
      m_loseTimer(0.0f), m_screamSoundPlayed(false), m_loseMusicPlayed(false)
{
    m_inGameMenu = std::make_unique<InGameMenu>(res);

    // Load House of Terror bitmap font for level labels
    m_font.Load(res.GetAssetPath("assets/data/HouseofTerror28.png"), res.GetAssetPath("assets/data/HouseofTerror28.txt"));

    // Initialize "READY... SET... PLANT!" intro animation
    ReanimDefinition readyDef = res.LoadReanim(res.GetAssetPath("assets/reanim/StartReadySetPlant.reanim"));
    m_readySetPlantAnim.SetResources(readyDef, res);
    m_readySetPlantAnim.SetLooping(false);

    // Clear grid
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
    }

    // Generate preview zombies and lawn mowers
    initPreviewZombies();
    initLawnMowers();
}

void Level1::restartLevel() {
    // Clear grid
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
    }

    m_zombies.clear();
    m_projectiles.clear();
    m_suns.clear();
    m_effects.clear();

    m_currentWave = 0;
    m_waveTimer = 14.0f;
    m_maxWaves = 5;
    m_skySunTimer = 0.0f;
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
    initPreviewZombies();
    initLawnMowers();
}

std::vector<std::string> Level1::getUniqueLevelZombieTypes() const {
    return { "ZombieNormal", "FlagZombie" };
}

void Level1::initPreviewZombies() {
    m_previewZombies.clear();
    std::vector<std::string> uniqueTypes = getUniqueLevelZombieTypes();

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    std::vector<int> availableRows = { 0, 1, 2, 3, 4 };
    for (size_t i = 0; i < uniqueTypes.size(); ++i) {
        std::string typeName = uniqueTypes[i];
        int row = availableRows[i % availableRows.size()];

        // Street world X range on background1.png (1400px wide): 1130.0f..1350.0f
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

    // Sort preview zombies by Y ascending so lower row zombies (higher Y) draw on top
    std::sort(m_previewZombies.begin(), m_previewZombies.end(),
        [](const PreviewZombieItem& a, const PreviewZombieItem& b) {
            return a.worldY < b.worldY;
        });
}

void Level1::initLawnMowers() {
    m_lawnMowers.clear();
    for (int r = 0; r < 5; ++r) {
        float mowerX = 65.0f;
        float mowerY = 80.0f + (float)r * 100.0f;
        m_lawnMowers.emplace_back(res, mowerX, mowerY, r);
    }
}

bool Level1::getGridCell(Vector2 mousePos, int& outRow, int& outCol) const {
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

void Level1::spawnSunFromSky() {
    float spawnX = (float)(220 + std::rand() % 500);
    float targetY = (float)(120 + std::rand() % 380);
    Texture2D tex = res.GetTexture("SUN");
    if (tex.id == 0) tex = res.GetTexture("Sun");
    m_suns.push_back(SunItem(spawnX, -40.0f, targetY, tex));
}

void Level1::createPlant(const std::string& type, int row, int col, int pixelX, int pixelY) {
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
    } else if (type == "TwinSunflower") {
        m_grid[row][col] = std::make_unique<TwinSunflower>(res, pixelX, pixelY);
    }
}

void Level1::spawnNextWave() {
    m_currentWave++;
    float spawnX = 830.0f;

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(3)));
    } else if (m_currentWave == 5) {
        m_finalWaveAnnounced = true;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(4)));
        return;
    }

    static const std::vector<std::string> waveGroanSounds = {
        "assets/sounds/sukhbir.ogg", "assets/sounds/sukhbir2.ogg", "assets/sounds/sukhbir3.ogg",
        "assets/sounds/groan.ogg", "assets/sounds/groan2.ogg", "assets/sounds/lowgroan.ogg"
    };
    int rIdx = GetRandomValue(0, (int)waveGroanSounds.size() - 1);
    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(waveGroanSounds[rIdx]));
}

void Level1::createSplat(float x, float y, bool isSnow) {
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

void Level1::createFireSplat(float x, float y) {
    Texture2D sparkTex = res.GetTexture("FirePea_spark");
    if (sparkTex.id == 0) sparkTex = res.GetTexture("FIREPEA_SPARK");

    int sparkCount = GetRandomValue(10, 14);
    for (int i = 0; i < sparkCount; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(-5, 5);
        p.y = y + (float)GetRandomValue(-5, 5);
        p.vx = (float)GetRandomValue(-160, 160);
        p.vy = (float)GetRandomValue(-200, 30);  // Fly outward & pop up
        p.gravity = 450.0f;                      // Pull sparks back down
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-600, 600);
        p.scale = (float)GetRandomValue(9, 18) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 3.2f;                       // Fades out in ~0.3s
        p.isPhysicsParticle = true;
        p.active = true;
        if (sparkTex.id != 0) {
            p.texture = sparkTex;
        }

        int c = GetRandomValue(0, 2);
        if (c == 0) p.tint = Color{ 255, 230, 80, 255 };      // Vivid Yellow / Gold
        else if (c == 1) p.tint = Color{ 255, 140, 20, 255 }; // Fiery Orange
        else p.tint = Color{ 255, 50, 10, 255 };              // Fiery Red

        m_effects.push_back(p);
    }
}

void Level1::createEatingParticle(float x, float y) {
    int count = GetRandomValue(2, 4);
    for (int i = 0; i < count; ++i) {
        ParticleEffect p;
        p.x = x + (float)GetRandomValue(0, 20);
        p.y = y + (float)GetRandomValue(-15, 10);
        p.vx = (float)GetRandomValue(-55, 55);
        p.vy = (float)GetRandomValue(-120, -40); // Pop upward
        p.gravity = 500.0f;                      // Gravity pulls down
        p.rotation = (float)GetRandomValue(0, 360);
        p.vr = (float)GetRandomValue(-360, 360);
        p.scale = (float)GetRandomValue(8, 14) / 10.0f;
        p.alpha = 1.0f;
        p.fadeRate = 2.5f; // Fades in ~0.4s
        p.isPhysicsParticle = true;
        p.active = true;

        int c = GetRandomValue(0, 2);
        if (c == 0) p.tint = Color{ 34, 177, 76, 255 };      // Vivid Green
        else if (c == 1) p.tint = Color{ 140, 210, 40, 255 }; // Light/Lime Green
        else p.tint = Color{ 20, 120, 40, 255 };              // Dark Leaf Green

        m_effects.push_back(p);
    }
}

void Level1::updateCollisions(float dt) {
    // 1. Projectiles vs Zombies
    for (auto& p : m_projectiles) {
        if (!p.isActive() || p.isImpacting()) continue;
        for (auto& z : m_zombies) {
            if (z->isDead()) continue;
            // Check Y lane match (within 55 pixels) and X collision
            float projBaseY = p.isLobbed() ? (p.getStartY() + 60.0f) : p.getY();
            if (std::abs(projBaseY - (z->getY() + 40.0f)) < 55.0f) {
                if (p.getX() >= z->getX() + 5.0f && p.getX() <= z->getX() + 45.0f) {
                    // Play projectile impact sound effects
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
                    
                    // Splash damage for Melonpult to nearby zombies
                    if (p.isMelon()) {
                        for (auto& otherZ : m_zombies) {
                            if (otherZ.get() == z.get() || otherZ->isDead()) continue;
                            float dx = otherZ->getX() - z->getX();
                            float dy = otherZ->getY() - z->getY();
                            if (dx * dx + dy * dy <= 120.0f * 120.0f) {
                                otherZ->takeDamage(26); // Melon splash damage ~26 HP
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

    // 1.8. Grid Plant Triggers vs Zombies
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
                        int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                        if (std::abs(zRow - r) <= 1) {
                            float zCx = z->getX() + 40.0f;
                            float zCy = z->getY() + 40.0f;
                            float dx = zCx - cbCx;
                            float dy = zCy - cbCy;
                            if (dx * dx + dy * dy <= 180.0f * 180.0f) {
                                z->takeExplosiveDamage(1800);
                            }
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
            }
        }
    }

    // 2. Zombies eating Plants
    for (auto& z : m_zombies) {
        if (z->isDead() || z->isSquashed()) continue;

        // Update Garlic bite timer and smooth lane diversion
        z->updateGarlicBite(dt);

        PoleVaultingZombie* pvz = dynamic_cast<PoleVaultingZombie*>(z.get());
        if (pvz && pvz->isVaulting()) {
            // In mid-air vaulting over plants: skip eating
            continue;
        }

        if (z->isChangingLane()) {
            // Actively shifting diagonally to the target lane: skip eating old-lane plants
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
                    continue; // Zombies cannot eat floor spike plants!
                }

                float plantX = (float)p->getX();

                // Trigger pole vault if zombie hasn't vaulted yet
                if (pvz && !pvz->hasVaulted() && !pvz->isVaulting()) {
                    if (z->getX() >= plantX - 10.0f && z->getX() <= plantX + 80.0f) {
                        pvz->startVault();
                        break;
                    }
                }

                if (z->getX() >= plantX - 20.0f && z->getX() <= plantX + 45.0f) {
                    if (p->getName() == "Garlic") {
                        if (!z->isBitingGarlic()) {
                            // First contact with Garlic: start the single bite sequence!
                            int nextRow = zRow;
                            if (zRow == 0) nextRow = 1;
                            else if (zRow == 4) nextRow = 3;
                            else nextRow = (GetRandomValue(0, 1) == 0) ? (zRow - 1) : (zRow + 1);

                            float targetY = 45.0f + nextRow * 100.0f;
                            z->startBitingGarlic(targetY, 0.55f);

                            // Deal single bite damage to Garlic
                            p->takeDamage(25.0f);
                            createEatingParticle(plantX + 25.0f, (float)p->getY() + 45.0f);

                            static const std::vector<std::string> chompSounds = {
                                "assets/sounds/chomp.ogg",
                                "assets/sounds/chomp2.ogg",
                                "assets/sounds/chompsoft.ogg"
                            };
                            int chompIdx = GetRandomValue(0, (int)chompSounds.size() - 1);
                            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(chompSounds[chompIdx]));

                            if (p->isDead()) {
                                m_grid[zRow][c] = nullptr;
                            }
                        }
                        foundPlantToEat = true;
                        break;
                    }

                    foundPlantToEat = true;
                    z->setEating(true);
                    if (z->getAnim().GetCurrentAnimName() != "anim_eat") {
                        z->getAnim().SetAnimation("anim_eat");
                    }

                    p->takeDamage((float)z->getDamage() * dt);

                    // Spawn eating food crumbs effect & play chomp sound periodically
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

        if (!foundPlantToEat && (!pvz || (!pvz->isVaulting() && pvz->hasVaulted())) && !z->isBitingGarlic() && !z->isChangingLane()) {
            if (z->isEating() || z->getAnim().GetCurrentAnimName() == "anim_eat") {
                z->setEating(false);
                z->resetEatTimer();
                z->getAnim().SetAnimation("anim_walk");
            }
        }

        // Check if zombie triggers lawnmower on this row
        for (auto& mower : m_lawnMowers) {
            if (mower.getRow() == zRow && !mower.isFinished()) {
                if (!mower.isTriggered() && z->getX() <= mower.getX() + 30.0f) {
                    mower.trigger();
                }
            }
        }

        // Check loss condition: Zombie reaches house door (x <= 20)
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

    // Process moving LawnMowers vs plants and zombies
    for (auto& mower : m_lawnMowers) {
        if (!mower.isTriggered() || mower.isFinished()) continue;
        int mRow = mower.getRow();
        float mX = mower.getX();

        // 1. Clear plants on this lane as mower rolls through
        for (int c = 0; c < 9; ++c) {
            float cellX = 140.0f + (c == 0 ? 0.0f : 80.0f + (c - 1) * 70.0f);
            if (mX >= cellX && m_grid[mRow][c] != nullptr) {
                m_grid[mRow][c] = nullptr;
            }
        }

        // 2. Crush and dismember zombies on this lane
        for (auto& z : m_zombies) {
            if (z->isDead() || z->isDevoured()) continue;
            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
            if (zRow == mRow) {
                if (std::abs(z->getX() - mX) <= 50.0f || (z->getX() <= mX && z->getX() >= mX - 60.0f)) {
                    z->takeDamage(1800.0f); // Fatal damage triggers decapitation / head & arms drop
                }
            }
        }
    }
}

void Level1::update(float dt) {
    // 0. Update in-game pause menu if open (pauses game loop simulation)
    if (m_inGameMenu && m_inGameMenu->isOpen()) {
        InGameMenuAction action = m_inGameMenu->update(dt);
        if (action == InGameMenuAction::RestartLevel) {
            restartLevel();
        } else if (action == InGameMenuAction::MainMenu) {
            m_exitToMainMenu = true;
        }
        return;
    }

    // Toggle menu via ESC key in any phase
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

    // Check click on top-right Menu button in any phase
    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    if (!m_levelWon && !m_levelLost && mouseClicked && CheckCollisionPointRec(mousePos, menuBtnRect)) {
        if (m_inGameMenu) m_inGameMenu->open();
        return;
    }

    if (m_phase == LevelPhase::SeedSelection) {
        m_cameraCropX = 500.0f;
        for (auto& item : m_previewZombies) {
            item.zombie->getAnim().Update(dt);
        }
        if (m_seedSelectMenu.update(dt, mousePos, mouseClicked)) {
            m_seedBank.initFromDeck(m_seedSelectMenu.getChosenDeck());
            m_phase = LevelPhase::PanToLawn;
            m_panTimer = 0.0f;
        }
        return;
    }

    if (m_phase == LevelPhase::PanToLawn) {
        for (auto& item : m_previewZombies) {
            item.zombie->getAnim().Update(dt);
        }
        m_panTimer += dt;
        float t = std::min(1.0f, m_panTimer / m_panDuration);
        float easeT = t * t * (3.0f - 2.0f * t); // Smooth-step cubic lerp
        m_cameraCropX = 500.0f + (90.0f - 500.0f) * easeT;

        if (t >= 1.0f) {
            m_cameraCropX = 90.0f;
            m_phase = LevelPhase::ReadySetPlant;
            m_previewZombies.clear(); // Clear preview zombies upon reaching active gameplay
            m_readySetPlantTimer = 0.0f;
            m_readySetPlantAnim.SetFrame(0.0f);
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/readysetplant.ogg"));
        }
        return;
    }

    if (m_phase == LevelPhase::ReadySetPlant) {
        m_readySetPlantTimer += dt;
        m_readySetPlantAnim.Update(dt);

        if (m_readySetPlantTimer >= 1.9f) {
            m_phase = LevelPhase::ActiveWave;
            AudioManager::GetInstance().PlayMusic(MusicTrack::DayLevel);
        }
        return;
    }

    // Handle Speed & Pause Buttons in ActiveWave phase
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
            int rewardCoins = (m_levelNumber == 1 ? 250 : (m_levelNumber == 2 ? 350 : 500));
            ProfileManager::GetInstance().AddCoins(rewardCoins);
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

    bool rightClicked = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    if (rightClicked) {
        Vector2 mousePos = GetVirtualMousePosition();
        int getRow, getCol;
        if (getGridCell(mousePos, getRow, getCol)) {
            m_zombies.push_back(std::make_unique<FlagZombie>(res, 700.0f, 45.0f + getRow * 100.0f));
        }
        m_seedBank.deselect();
    }

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c] && m_grid[r][c]->isDead()) {
                m_grid[r][c] = nullptr;
            } else if (m_grid[r][c]) {
                bool shoot = false;
                float plantX = (float)m_grid[r][c]->getX();
                float plantY = (float)m_grid[r][c]->getY();

                float min_distance = 800.0f; // Only consider zombies within 700 pixels in front of the plant
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
                if (plantName == "CherryBomb" || plantName == "Jalapeno" || plantName == "PotatoMine" || plantName == "Squash" || plantName == "IceShroom" || plantName == "Chomper" || plantName == "Caltrop" || plantName == "SpikeRock") {
                    continue; // Let self-animated/trigger plants manage their own animations
                }

                std::string targetAnim;
                if (shoot) {
                    targetAnim = (plantName == "SunFlower" || plantName == "TwinSunflower" || plantName == "Wallnut" ||
                                  plantName == "Garlic" || plantName == "Gravebuster" ||
                                  plantName == "Caltrop" || plantName == "SpikeRock" ||
                                  plantName == "Chomper") ? "anim_idle" : "anim_shooting";
                } else {
                    targetAnim = (plantName == "SunFlower" || plantName == "TwinSunflower" || plantName == "Wallnut" ||
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
                    if ((plantName == "SunFlower" || plantName == "TwinSunflower") && targetAnim == "anim_idle") {
                        m_grid[r][c]->SetBaseAnimation("anim_idle");
                    }
                }
            }
        }
    }

    m_seedBank.update(simDt, mousePos, mouseClicked);

    // Plant placement / Shovel removal logic
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
                if (selectedType == "Gravebuster") {
                    // Day levels (Level 1-3) have no graves to bust
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
                } else if (m_grid[hoverRow][hoverCol] == nullptr || (selectedType == "TwinSunflower" && m_grid[hoverRow][hoverCol] && m_grid[hoverRow][hoverCol]->getName() == "SunFlower")) {
                    float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
                    float cellH = 100.0f;
                    float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
                    float cellY = 80.0f + hoverRow * 100.0f;
                    float centerX = cellX + cellW / 2.0f;
                    float centerY = cellY + cellH / 2.0f;
                    int px = (int)(centerX - 30.0f - 10.0f);
                    int py = (int)(centerY - 35.0f );
                    createPlant(selectedType, hoverRow, hoverCol, px, py);
                    m_seedBank.consumeSelected();

                    int plantSfxChoice = GetRandomValue(0, 1);
                    std::string plantSfx = (plantSfxChoice == 0) ? "assets/sounds/plant.ogg" : "assets/sounds/plant2.ogg";
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(plantSfx));
                } else {
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/buzzer.ogg"));
                }
            }
        }
    }

    // Click collection for sun items
    if (mouseClicked) {
        for (auto& s : m_suns) {
            if (s.isActive() && s.isClicked(mousePos)) {
                s.collect(20.0f, 15.0f);
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/points.ogg"));
            }
        }
    }

    // Sub-stepping simulation loop:
    // Fixed sub-step size of ~1/60s (0.0166f) ensures rock-solid collision detection & physics at 2x, 4x, 8x speeds.
    float remainingDt = simDt;
    const float maxSubStep = 0.0166f;

    while (remainingDt > 0.0f) {
        float subDt = std::min(remainingDt, maxSubStep);
        remainingDt -= subDt;

        // Sky sun timer
        m_skySunTimer += subDt;
        if (m_skySunTimer >= 10.0f) {
            m_skySunTimer = 0.0f;
            spawnSunFromSky();
        }

        // Wave spawn timer
        if (m_currentWave < m_maxWaves) {
            m_waveTimer -= subDt;
            if (m_waveTimer <= 0.0f) {
                spawnNextWave();
                m_waveTimer = 22.0f;
            }
        }

        // Update plants
        for (int r = 0; r < 5; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                    m_grid[r][c]->update(subDt, m_projectiles, m_suns);
                }
            }
        }

        // Update zombies
        for (auto& z : m_zombies) {
            if (!z->isFinished()) {
                z->update(subDt);
            }
        }

        // Update projectiles
        for (auto& p : m_projectiles) {
            p.update(subDt);
        }

        // Update lawn mowers
        for (auto& mower : m_lawnMowers) {
            mower.update(subDt);
        }

        // Update sun items
        for (auto& s : m_suns) {
            s.update(subDt);
            if (s.hasArrived()) {
                m_seedBank.addSun(25);
            }
        }

        // Update particle / splat / crumb effects
        for (auto& eff : m_effects) {
            if (!eff.active) continue;
            eff.timing(subDt);
        }

        // Clean up inactive projectiles, suns, finished zombies, and particle effects
        m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }), m_projectiles.end());

        m_suns.erase(std::remove_if(m_suns.begin(), m_suns.end(),
            [](const SunItem& s) { return !s.isActive(); }), m_suns.end());

        m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
            [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }), m_zombies.end());

        m_effects.erase(std::remove_if(m_effects.begin(), m_effects.end(),
            [](const ParticleEffect& e) { return !e.isActive(); }), m_effects.end());

        // Update collisions per sub-step
        updateCollisions(subDt);
    }

    // Check win condition
    if (m_currentWave >= m_maxWaves) {
        bool anyZombieAlive = false;
        for (const auto& z : m_zombies) {
            if (!z->isDead()) {
                anyZombieAlive = true;
                break;
            }
        }
        if (!anyZombieAlive) {
            m_victoryDelayTimer += simDt;
            if (m_victoryDelayTimer >= 2.0f) {
                m_levelWon = true;
            }
        } else {
            m_victoryDelayTimer = 0.0f;
        }
    }
}

void Level1::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Background cropped at {m_cameraCropX, 0, 900, 600}
    Texture2D bgTex = res.GetBackground();
    if (bgTex.id != 0) {
        DrawTexturePro(
            bgTex,
            { m_cameraCropX, 0.0f, 900.0f, 600.0f },
            { 0.0f, 0.0f, 800.0f, 600.0f },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        // Fallback lawn grid rendering
        for (int y = 0; y < 600; y += 80) {
            for (int x = 0; x < 800; x += 80) {
                DrawRectangle(x, y, 80, 80, ((x/80 + y/80) % 2 == 0) ? GREEN : DARKGREEN);
            }
        }
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
        // Draw SeedBank
        m_seedBank.draw(res, mousePos);
        // Draw "READY... SET... PLANT!" animated banner centered on screen
        m_readySetPlantAnim.Draw(400.0f, 300.0f, 1.0f);
    } else {
        // 2. Draw hover highlight cell on lawn grid
        int hoverRow, hoverCol;
        if (getGridCell(mousePos, hoverRow, hoverCol)) {
            float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
            float cellY = 80.0f + hoverRow * 100.0f;
            float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
            float cellH = 100.0f;
            std::string selType = m_seedBank.getSelectedPlantType();
            bool isValid = (selType != "Gravebuster") && (m_grid[hoverRow][hoverCol] == nullptr);
            DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, isValid ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(RED, 0.6f));
        }

        // 3. Draw Entities Row-by-Row from Top (Row 0, Y lowest) to Bottom (Row 4, Y highest)
        // This ensures entities in lower rows (higher Y) always render on top of entities above them.
        for (int r = 0; r < 5; ++r) {
            // A. LawnMower in row r
            for (const auto& mower : m_lawnMowers) {
                if (mower.getRow() == r) {
                    mower.draw();
                }
            }

            // B. Plants in row r
            for (int c = 0; c < 9; ++c) {
                if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                    m_grid[r][c]->draw();
                }
            }

            // C. Zombies in row r
            for (const auto& z : m_zombies) {
                if (!z->isFinished()) {
                    int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                    if (zRow == r) {
                        z->draw();
                    }
                }
            }
        }

        // Draw any zombies whose row is outside 0..4 (fallback safety)
        for (const auto& z : m_zombies) {
            if (!z->isFinished()) {
                int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                if (zRow < 0 || zRow > 4) {
                    z->draw();
                }
            }
        }

        // 4. Draw Top SeedBank & Plant Seed Packets UI
        m_seedBank.draw(res, mousePos);

        // 5. Draw Bottom-Right FlagMeter Progress Bar
        drawProgressBar();

        // 6. Draw Speed & Pause Controls
        drawSpeedControls();

        // 7. Draw Projectiles (drawn on top of SeedBank for high lobbed arcs)
        for (const auto& p : m_projectiles) {
            p.draw();
        }

        // 8. Draw Particle & Splat Effects
        for (const auto& eff : m_effects) {
            eff.draw();
        }

        // 9. Draw Sun Items
        for (const auto& s : m_suns) {
            s.draw();
        }

        // 10. Draw Win / Loss Overlays
        if (m_levelWon) {
            drawWinScreen();
        } else if (m_levelLost) {
            drawLoseScreen();
        }
    }

    // 11. Draw top-right "Menu" stone button (680, 0, 110, 36) in all phases
    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    bool menuHovered = CheckCollisionPointRec(mousePos, menuBtnRect);
    bool menuPressed = menuHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (m_inGameMenu) {
        m_inGameMenu->drawMenuButton(menuHovered, menuPressed);
    }

    // 12. Draw in-game pause menu dialog if open (on top of all phases)
    if (m_inGameMenu && m_inGameMenu->isOpen()) {
        m_inGameMenu->draw();
    }

    EndTextureMode();

    // 13. Draw stretched to screen
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

void Level1::drawProgressBar() {
    Texture2D texMeter = res.GetTexture("FLAGMETER");
    Texture2D texBadge = res.GetTexture("FLAGMETERLEVELPROGRESS");
    Texture2D texParts = res.GetTexture("FLAGMETERPARTS");

    if (texMeter.id == 0) return;

    // Progress computation (0.0 to 1.0)
    float waveProgress = (float)m_currentWave / (float)std::max(1, m_maxWaves);
    if (m_currentWave < m_maxWaves) {
        float waveFraction = std::clamp((22.0f - m_waveTimer) / 22.0f, 0.0f, 1.0f);
        waveProgress = ((float)m_currentWave + waveFraction) / (float)m_maxWaves;
    }
    waveProgress = std::clamp(waveProgress, 0.0f, 1.0f);

    float barX = 625.0f;
    float barY = 572.0f;

    // 1. Draw level text to the left: "Level 1", "Level 2", "Level 3"
    std::string levelStr = "Level " + std::to_string(m_levelNumber);
    float fontScale = 0.65f;
    int textW = m_font.MeasureText(levelStr.c_str(), fontScale);
    float textX = barX - (float)textW - 10.0f;
    float textY = 566.0f;

    // Drop shadow
    m_font.DrawText(levelStr.c_str(), textX + 2.0f, textY + 2.0f, fontScale, Color{ 0, 0, 0, 255 });
    // Golden yellow text
    m_font.DrawText(levelStr.c_str(), textX, textY, fontScale, Color{ 235, 200, 45, 255 });

    // 2. Draw Progress Bar Frame (0, 0, 158, 25)
    Rectangle srcFrame = { 0.0f, 0.0f, 158.0f, 25.0f };
    DrawTextureRec(texMeter, srcFrame, { barX, barY }, WHITE);

    // 3. Draw Green Progress Fill (filling from right to left)
    // Slot width = 149 (from x=6 to x=155)
    float maxFillWidth = 149.0f;
    float currentFillWidth = maxFillWidth * waveProgress;
    if (currentFillWidth > 0.0f) {
        Rectangle srcFill = { 155.0f - currentFillWidth, 27.0f, currentFillWidth, 24.0f };
        Vector2 destFillPos = { barX + 155.0f - currentFillWidth, barY };
        DrawTextureRec(texMeter, srcFill, destFillPos, WHITE);
    }

    // 4. Draw "LEVEL PROGRESS" badge in the lower middle slot
    if (texBadge.id != 0) {
        DrawTextureRec(texBadge, { 0.0f, 0.0f, (float)texBadge.width, (float)texBadge.height }, { barX + 36.0f, barY + 13.0f }, WHITE);
    }

    // 5. Draw Red Flags at wave milestones
    if (texParts.id != 0) {
        std::vector<float> flagFractions;
        if (m_levelNumber == 3) {
            flagFractions = { 0.50f, 1.0f }; // Flag 1 at wave 5, Flag 2 at wave 10
        } else {
            flagFractions = { 1.0f };        // Final wave only
        }

        for (float frac : flagFractions) {
            float flagX = barX + 155.0f - maxFillWidth * frac - 8.0f;
            flagX = std::max(barX + 6.0f, flagX);

            // Flag pole
            Rectangle srcPole = { 25.0f, 0.0f, 25.0f, 25.0f };
            DrawTextureRec(texParts, srcPole, { flagX, barY - 2.0f }, WHITE);

            // Red flag (raised when near/reached that wave fraction)
            float flagOffsetY = (waveProgress >= frac - 0.05f) ? -6.0f : -2.0f;
            Rectangle srcFlag = { 50.0f, 0.0f, 25.0f, 25.0f };
            DrawTextureRec(texParts, srcFlag, { flagX, barY + flagOffsetY }, WHITE);
        }

        // 6. Draw Zombie Head Slider Marker
        float headX = barX + 155.0f - currentFillWidth - 11.0f;
        headX = std::clamp(headX, barX + 6.0f, barX + 144.0f);
        Rectangle srcHead = { 0.0f, 0.0f, 25.0f, 25.0f };
        DrawTextureRec(texParts, srcHead, { headX, barY - 2.0f }, WHITE);
    }
}

void Level1::drawSpeedControls() {
    Vector2 mousePos = GetVirtualMousePosition();
    Rectangle pauseBtn = { 668.0f, 538.0f, 26.0f, 26.0f };
    Rectangle speedBtn = { 698.0f, 538.0f, 85.0f, 26.0f };

    bool pauseHover = CheckCollisionPointRec(mousePos, pauseBtn);
    bool speedHover = CheckCollisionPointRec(mousePos, speedBtn);

    // 1. Draw Pause Button
    DrawRectangleRounded(pauseBtn, 0.25f, 4, pauseHover ? Color{ 85, 95, 135, 255 } : Color{ 60, 68, 105, 255 });
    DrawRectangleRoundedLines(pauseBtn, 0.25f, 4, 1.5f, pauseHover ? Color{ 140, 160, 220, 255 } : Color{ 100, 115, 165, 255 });
    if (m_isSpeedPaused) {
        DrawTriangle({ pauseBtn.x + 8.0f, pauseBtn.y + 6.0f },
                     { pauseBtn.x + 8.0f, pauseBtn.y + pauseBtn.height - 6.0f },
                     { pauseBtn.x + pauseBtn.width - 7.0f, pauseBtn.y + pauseBtn.height / 2.0f },
                     Color{ 240, 240, 255, 255 });
    } else {
        DrawRectangleRec({ pauseBtn.x + 7.0f, pauseBtn.y + 6.0f, 4.0f, 14.0f }, Color{ 220, 230, 255, 255 });
        DrawRectangleRec({ pauseBtn.x + 15.0f, pauseBtn.y + 6.0f, 4.0f, 14.0f }, Color{ 220, 230, 255, 255 });
    }

    // 2. Draw Speed Button
    DrawRectangleRounded(speedBtn, 0.25f, 4, speedHover ? Color{ 85, 95, 135, 255 } : Color{ 60, 68, 105, 255 });
    DrawRectangleRoundedLines(speedBtn, 0.25f, 4, 1.5f, speedHover ? Color{ 140, 160, 220, 255 } : Color{ 100, 115, 165, 255 });
    DrawTriangle({ speedBtn.x + 8.0f, speedBtn.y + 7.0f },
                 { speedBtn.x + 8.0f, speedBtn.y + speedBtn.height - 7.0f },
                 { speedBtn.x + 19.0f, speedBtn.y + speedBtn.height / 2.0f },
                 Color{ 220, 235, 255, 255 });

    char speedBuf[16];
    snprintf(speedBuf, sizeof(speedBuf), "%.0fx", m_gameSpeed);
    DrawText(speedBuf, (int)(speedBtn.x + 24.0f), (int)(speedBtn.y + 4.0f), 17, (m_gameSpeed > 1.0f) ? Color{ 255, 220, 40, 255 } : Color{ 230, 235, 245, 255 });
}

void Level1::drawWinScreen() {
    float overlayAlpha = std::clamp(m_winTimer * 1.5f, 0.0f, 0.75f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(BLACK, overlayAlpha));

    Texture2D texTrophy = res.GetTexture("TROPHY_HI_RES");
    if (texTrophy.id == 0) texTrophy = res.GetTexture("TROPHY");

    float centerX = 400.0f;
    float centerY = m_awardY;

    // Draw rotating sun rays behind trophy
    Texture2D texRaysImg = res.GetTexture("AWARDRAYS");
    if (texRaysImg.id != 0) {
        Rectangle srcRays = { 0, 0, (float)texRaysImg.width, (float)texRaysImg.height };
        Rectangle destRays = { centerX, centerY, 320.0f, 320.0f };
        Vector2 originRays = { 160.0f, 160.0f };
        DrawTexturePro(texRaysImg, srcRays, destRays, originRays, m_awardRaysRotation, ColorAlpha(WHITE, 0.85f));
    }

    // Draw Trophy
    if (texTrophy.id != 0) {
        Rectangle srcTrophy = { 0, 0, (float)texTrophy.width, (float)texTrophy.height };
        Rectangle destTrophy = { centerX, centerY, 160.0f, 160.0f };
        Vector2 originTrophy = { 80.0f, 80.0f };
        DrawTexturePro(texTrophy, srcTrophy, destTrophy, originTrophy, 0.0f, WHITE);
    }

    // Draw Award Banner and text when trophy lands
    if (m_winTimer >= 1.0f) {
        float bannerAlpha = std::clamp((m_winTimer - 1.0f) * 2.0f, 0.0f, 1.0f);
        DrawRectangleRec({ 200, 360, 400, 150 }, ColorAlpha(Color{ 20, 25, 40, 255 }, bannerAlpha * 0.90f));
        DrawRectangleLinesEx({ 200, 360, 400, 150 }, 3.0f, ColorAlpha(GOLD, bannerAlpha));
        DrawText("LEVEL COMPLETED!", 260, 385, 28, ColorAlpha(GOLD, bannerAlpha));
        DrawText("You defeated all zombies and saved your lawn!", 225, 428, 17, ColorAlpha(WHITE, bannerAlpha));
        DrawText("Click anywhere or press ENTER to continue", 230, 465, 16, ColorAlpha(LIGHTGRAY, bannerAlpha));
    }
}

void Level1::drawLoseScreen() {
    // 1. Fade to dark red / black vignette
    float overlayAlpha = std::clamp(m_loseTimer * 1.8f, 0.0f, 0.88f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(Color{ 25, 0, 0, 255 }, overlayAlpha));

    // 2. Draw "THE ZOMBIES ATE YOUR BRAINS!" graphic with zoom-in ease-out
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

    // 3. Prompt when defeat animation concludes
    if (m_loseTimer >= 1.6f) {
        float textAlpha = std::clamp((m_loseTimer - 1.6f) * 2.0f, 0.0f, 1.0f);
        DrawRectangleRec({ 245, 470, 310, 44 }, ColorAlpha(BLACK, textAlpha * 0.75f));
        DrawRectangleLinesEx({ 245, 470, 310, 44 }, 2.0f, ColorAlpha(RED, textAlpha));
        DrawText("Click anywhere to try again", 270, 483, 18, ColorAlpha(RAYWHITE, textAlpha));
    }
}

void Level1::run() {
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
