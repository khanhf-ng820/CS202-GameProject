#include "BowlingLevel.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include "ProfileManager.h"

BowlingLevel::BowlingLevel(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen) {
    m_inGameMenu = std::make_unique<InGameMenu>(res);
    m_font.Load(res.GetAssetPath("assets/data/HouseofTerror28.png"), res.GetAssetPath("assets/data/HouseofTerror28.txt"));

    std::string briannePng = res.GetAssetPath("assets/data/_BrianneTod16.png");
    std::string brianneTxt = res.GetAssetPath("assets/data/BrianneTod16.txt");
    if (FileExists(briannePng.c_str()) && FileExists(brianneTxt.c_str())) {
        m_brianneLoaded = m_brianneFont.Load(briannePng, brianneTxt);
    }

    ReanimDefinition readyDef = res.LoadReanim(res.GetAssetPath("assets/reanim/StartReadySetPlant.reanim"));
    m_readySetPlantAnim.SetResources(readyDef, res);
    m_readySetPlantAnim.SetLooping(false);
    m_readySetPlantTimer = 0.0f;
    m_readySetPlantDone = false;
    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/readysetplant.ogg"));

    initLawnMowers();
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
    }
}

void BowlingLevel::initLawnMowers() {
    m_lawnMowers.clear();
    for (int r = 0; r < 5; ++r) {
        float mowerX = 65.0f;
        float mowerY = 80.0f + (float)r * 100.0f;
        m_lawnMowers.emplace_back(res, mowerX, mowerY, r);
    }
}

void BowlingLevel::restartLevel() {
    m_currentWave = 0;
    m_waveTimer = 2.0f;
    m_finalWaveAnnounced = false;
    m_levelWon = false;
    m_levelLost = false;
    m_exitToMainMenu = false;
    m_gameSpeed = 1.0f;
    m_isSpeedPaused = false;
    m_loseTimer = 0.0f;
    m_screamSoundPlayed = false;
    m_loseMusicPlayed = false;
    m_winTimer = 0.0f;
    m_awardY = -100.0f;
    m_awardRaysRotation = 0.0f;
    m_winMusicPlayed = false;

    initLawnMowers();
    m_zombies.clear();
    m_bowlingNuts.clear();
    m_hitDebugTimers.clear();
    m_cards.clear();
    m_cardSpawnTimer = 0.0f;
    m_isHoldingCard = false;
    m_heldPlantType = "";

    m_readySetPlantTimer = 0.0f;
    m_readySetPlantDone = false;
    m_readySetPlantAnim.SetFrame(0.0f);
    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/readysetplant.ogg"));

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
    }
}

bool BowlingLevel::getGridCell(Vector2 mousePos, int& outRow, int& outCol) const {
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

void BowlingLevel::spawnNextWave() {
    m_currentWave++;
    float spawnX = 830.0f;

    auto laneY = [](int row) -> float {
        return 50.0f + row * 100.0f;
    };

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 40.0f, laneY(0)));
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 30.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 60.0f, laneY(4)));
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 30.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 50.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 80.0f, laneY(0)));
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 40.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 70.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 90.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 110.0f, laneY(4)));
    } else if (m_currentWave == 5) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 30.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 50.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 80.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 100.0f, laneY(4)));
    } else if (m_currentWave == 6) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 40.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 60.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 80.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 100.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 120.0f, laneY(2)));
    } else if (m_currentWave == 7) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 30.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 50.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 70.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 90.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 110.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 130.0f, laneY(2)));
    } else if (m_currentWave == 8) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 30.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 50.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 70.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 90.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 110.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 130.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 150.0f, laneY(2)));
    } else if (m_currentWave == 9) {
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 30.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 50.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 70.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 90.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 110.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 130.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 150.0f, laneY(4)));
    } else if (m_currentWave == 10) {
        // Final wave! HUGE WAVE OF ZOMBIES!
        m_finalWaveAnnounced = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 30.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 40.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 50.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 60.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 70.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 80.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 90.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 100.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 110.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 120.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 130.0f, laneY(2)));
    }
}

void BowlingLevel::update(float dt) {
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

    if (!m_readySetPlantDone) {
        m_readySetPlantTimer += dt;
        m_readySetPlantAnim.Update(dt);
        if (m_readySetPlantTimer >= 1.9f) {
            m_readySetPlantDone = true;
            AudioManager::GetInstance().PlayMusic(MusicTrack::DayLevel);
        }
        return;
    }

    // Toggle menu via ESC key when playing
    if (!m_levelWon && !m_levelLost && IsKeyPressed(KEY_ESCAPE)) {
        if (m_inGameMenu) m_inGameMenu->open();
        return;
    }

    Vector2 mousePos = GetVirtualMousePosition();

    // Check click on top-right Menu button
    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    if (!m_levelWon && !m_levelLost && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, menuBtnRect)) {
        if (m_inGameMenu) m_inGameMenu->open();
        return;
    }

    // Handle Speed & Pause Buttons
    Rectangle pauseBtn = { 668.0f, 538.0f, 26.0f, 26.0f };
    Rectangle speedBtn = { 698.0f, 538.0f, 85.0f, 26.0f };

    if (!m_levelWon && !m_levelLost) {
        if (IsKeyPressed(KEY_SPACE) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, pauseBtn))) {
            m_isSpeedPaused = !m_isSpeedPaused;
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/pause.ogg"));
            return;
        }

        if (IsKeyPressed(KEY_F) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, speedBtn))) {
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
            ProfileManager::GetInstance().AddCoins(2000);
            AudioManager::GetInstance().PlayMusic(MusicTrack::None);
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/winmusic.ogg"));
        }

        if (m_winTimer >= 1.5f && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
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

        if (m_loseTimer >= 2.0f && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE))) {
            restartLevel();
        }
        return;
    }

    float simDt = m_isSpeedPaused ? 0.0f : dt * m_gameSpeed;

    // 0. Wave spawn timer (Aggressive 12.0s interval)
    if (m_currentWave < m_maxWaves) {
        m_waveTimer -= simDt;
        if (m_waveTimer <= 0.0f) {
            spawnNextWave();
            m_waveTimer = 12.0f;
        }
    }

    // 1. Advance conveyor belt animation frame (6 rows of 16px each in ConveyorBelt.png)
    m_animTimer += simDt;
    float frameDuration = 0.08f; // ~12.5 FPS animation speed
    if (m_animTimer >= frameDuration) {
        m_animTimer -= frameDuration;
        m_currentFrame = (m_currentFrame + 1) % 6;
    }

    // 2. Spawn a Wall-nut card every 3.8 seconds
    float spawnX = 459.0f;
    float leftMinX = 9.0f;
    float cardW = 50.0f;

    m_cardSpawnTimer += simDt;
    if (m_cardSpawnTimer >= 6.5f) {
        // Only spawn a new card if the conveyor belt has room (last card has moved left of spawn position)
        if (m_cards.empty() || m_cards.back().x < spawnX) {
            int roll = GetRandomValue(1, 100);
            std::string plantType = "Wallnut";
            if (roll <= 18) {
                plantType = "GiantWallnut";
            } else if (roll <= 38) {
                plantType = "ExplodeNut";
            }
            m_cards.push_back({ spawnX, plantType });
            m_cardSpawnTimer = 0.0f;
        } else {
            // Conveyor belt is full; cap timer at 6.5s so a card spawns immediately when space opens up
            m_cardSpawnTimer = 6.5f;
        }
    }

    // 3. Move cards leftward toward left end of conveyor belt (leftMinX = 9.0f) with 0 spacing
    float cardSpeed = 60.0f; // 60 px/s
    for (size_t i = 0; i < m_cards.size(); ++i) {
        float targetX = (i == 0) ? leftMinX : (m_cards[i - 1].x + cardW);
        if (m_cards[i].x > targetX) {
            m_cards[i].x -= cardSpeed * simDt;
            if (m_cards[i].x < targetX) {
                m_cards[i].x = targetX;
            }
        } else if (m_cards[i].x < targetX) {
            m_cards[i].x = targetX;
        }
    }

    // 6. Handle card pickup from conveyor belt (when not currently holding a card)
    if (!m_isHoldingCard && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (size_t i = 0; i < m_cards.size(); ++i) {
            Rectangle cardRect = { m_cards[i].x, 8.0f, 50.0f, 70.0f };
            if (CheckCollisionPointRec(mousePos, cardRect)) {
                m_isHoldingCard = true;
                m_heldPlantType = m_cards[i].plantType;
                m_cards.erase(m_cards.begin() + i);
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/seedlift.ogg"));
                break;
            }
        }
    }

    // 7. Handle plant placement on lawn grid (when holding a card)
    // Note: Deselecting is disabled per requirements — player MUST place the card!
    if (m_isHoldingCard && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int r, c;
        if (getGridCell(mousePos, r, c)) {
            // ONLY ALLOW placement on tiles to the left of the red bowling stripe (columns 0, 1, 2)
            // Tile remains immediately available for future placements (m_grid is not occupied)
            if (c <= 2) {
                float cellW = (c == 0) ? 80.0f : 70.0f;
                float cellH = 100.0f;
                float cellX = 140.0f + (c == 0 ? 0.0f : 80.0f + (c - 1) * 70.0f);
                float cellY = 80.0f + r * 100.0f;
                float centerX = cellX + cellW / 2.0f;
                float centerY = cellY + cellH / 2.0f;

                // Spawn rolling bowling nut using factory method
                m_bowlingNuts.push_back(BowlingNut::Create(m_heldPlantType, centerX, centerY));

                m_isHoldingCard = false;
                m_heldPlantType = "";
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/plant.ogg"));
            }
        }
    }

    // 8. Update rolling bowling nuts & handle zombie collisions + boundary bouncing
    for (auto& nut : m_bowlingNuts) {
        nut->update(simDt, m_zombies, m_hitDebugTimers, res);
    }

    // Update hit debug timers (decrement and cleanup)
    for (auto& item : m_hitDebugTimers) {
        item.second -= simDt;
    }
    m_hitDebugTimers.erase(
        std::remove_if(m_hitDebugTimers.begin(), m_hitDebugTimers.end(),
            [](const std::pair<const Zombie*, float>& item) { return item.second <= 0.0f; }),
        m_hitDebugTimers.end()
    );

    // Despawn bowling nuts that exit the screen on the right (x > 850.0f) or explode on impact
    m_bowlingNuts.erase(
        std::remove_if(m_bowlingNuts.begin(), m_bowlingNuts.end(),
            [](const std::unique_ptr<BowlingNut>& nut) { return nut->isOffScreen() || nut->isExploded(); }),
        m_bowlingNuts.end()
    );

    // 9. Update lawn mowers
    for (auto& mower : m_lawnMowers) {
        mower.update(simDt);
    }

    // 10. Update zombies (right to left movement) and check mower trigger / loss condition
    for (auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->update(simDt);

            int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);

            // Check if zombie triggers lawnmower on this row
            for (auto& mower : m_lawnMowers) {
                if (mower.getRow() == zRow && !mower.isFinished()) {
                    if (!mower.isTriggered() && z->getX() <= mower.getX() + 30.0f) {
                        mower.trigger();
                    }
                }
            }

            // Check loss condition: Zombie reaches house door (x <= 20.0f)
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
    }

    // 11. Process moving LawnMowers vs plants and zombies
    for (auto& mower : m_lawnMowers) {
        if (!mower.isTriggered() || mower.isFinished()) continue;
        int mRow = mower.getRow();
        float mX = mower.getX();

        // Clear plants on this lane as mower rolls through
        for (int c = 0; c < 9; ++c) {
            float cellX = 140.0f + (c == 0 ? 0.0f : 80.0f + (c - 1) * 70.0f);
            if (mX >= cellX && m_grid[mRow][c] != nullptr) {
                m_grid[mRow][c] = nullptr;
            }
        }

        // Crush and eliminate zombies on this lane
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

    // Clean up finished zombies
    m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
        [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }), m_zombies.end());

    // Check win condition: All waves spawned and all zombies defeated (with 2s victory delay)
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

    // 12. Update placed plants (if any)
    std::vector<Projectile> dummyProjectiles;
    std::vector<SunItem> dummySuns;
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                m_grid[r][c]->update(simDt, dummyProjectiles, dummySuns);
            }
        }
    }
}

void BowlingLevel::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Daytime Front Lawn Background
    Texture2D bgTex = res.GetBackground();
    if (bgTex.id != 0) {
        DrawTexturePro(
            bgTex,
            { 90.0f, 0.0f, 900.0f, 600.0f },
            { 0.0f, 0.0f, 800.0f, 600.0f },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        // Fallback lawn grid rendering
        for (int y = 0; y < 600; y += 80) {
            for (int x = 0; x < 800; x += 80) {
                DrawRectangle(x, y, 80, 80, ((x / 80 + y / 80) % 2 == 0) ? GREEN : DARKGREEN);
            }
        }
    }

    // 2. Draw Red Wall-nut Bowling Stripe between 3rd and 4th columns (x = 360.0f) across all 5 lawn rows (y = 80 to 580)
    Texture2D stripeTex = res.GetTexture("WALLNUT_BOWLINGSTRIPE");
    if (stripeTex.id == 0) stripeTex = res.GetTexture("Wallnut_bowlingstripe");

    float stripeW = (stripeTex.id != 0) ? (float)stripeTex.width : 22.0f;
    float stripeH = 500.0f; // Spans all 5 rows (80.0f to 580.0f)
    float stripeX = 360.0f - (stripeW / 2.0f);
    float stripeY = 80.0f;

    if (stripeTex.id != 0) {
        DrawTexturePro(
            stripeTex,
            { 0.0f, 0.0f, (float)stripeTex.width, (float)stripeTex.height },
            { stripeX, stripeY, stripeW, stripeH },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 3. Draw Entities Row-by-Row from Top (Row 0) to Bottom (Row 4)
    for (int r = 0; r < 5; ++r) {
        // A. LawnMower in row r
        for (const auto& mower : m_lawnMowers) {
            if (mower.getRow() == r) {
                mower.draw();
            }
        }

        // B. Placed plants in row r
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c]) {
                m_grid[r][c]->draw();
            }
        }
    }

    // 4. Draw active zombies & debug overlays (bounding boxes, center points, collision zones, hit highlight)
    for (const auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->draw();

            if (m_showDebug && !z->isDead()) {
                float zCx = z->getX() + 40.0f;
                float zCy = z->getY() + 80.0f; // Align collision center Y (130.0f + row * 100.0f) with Wall-nut center Y

                bool isHit = false;
                for (const auto& item : m_hitDebugTimers) {
                    if (item.first == z.get()) {
                        isHit = true;
                        break;
                    }
                }

                Color debugColor = isHit ? BLUE : RED;

                // Draw semi-transparent collision zone (R = 42.0f, front approach dx <= 10.0f: sector from 76.23° to 283.77°)
                DrawCircleSector({ zCx, zCy }, 42.0f, 76.23f, 283.77f, 36, ColorAlpha(debugColor, 0.35f));

                // Draw red/blue bounding box around zombie (80x100px)
                DrawRectangleLinesEx({ z->getX(), z->getY(), 80.0f, 100.0f }, 2.0f, debugColor);

                // Draw center point of zombie
                DrawCircle((int)zCx, (int)zCy, 4.0f, debugColor);
            }
        }
    }

    // 5. Draw active rolling Wall-nut bowling entities rotating around center & debug overlays
    for (const auto& nut : m_bowlingNuts) {
        nut->draw(res, m_showDebug);
    }

    // Draw top-right "Menu" stone button (680, 0, 110, 36)
    Vector2 mousePos = GetVirtualMousePosition();
    Rectangle menuBtnRect = InGameMenu::GetMenuButtonRect();
    bool menuHovered = CheckCollisionPointRec(mousePos, menuBtnRect);
    bool menuPressed = menuHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (m_inGameMenu) {
        m_inGameMenu->drawMenuButton(menuHovered, menuPressed);
    }

    // 6. Lawn grid hover logic (no rectangle outline overlay)

    // 7. Draw ConveyorBelt_backdrop at (0,0) (matching Level 1 SeedBank position)
    Texture2D backdropTex = res.GetTexture("CONVEYORBELT_BACKDROP");
    if (backdropTex.id == 0) backdropTex = res.GetTexture("ConveyorBelt_backdrop");
    if (backdropTex.id != 0) {
        DrawTexture(backdropTex, 0, 0, WHITE);
    }

    // Draw animated moving conveyor belt strip (502x16px per frame) inside backdrop channel (x=7, y=63)
    Texture2D conveyorTex = res.GetTexture("CONVEYORBELT");
    if (conveyorTex.id == 0) conveyorTex = res.GetTexture("ConveyorBelt");
    if (conveyorTex.id != 0) {
        float srcY = (float)m_currentFrame * 16.0f;
        Rectangle srcRec = { 0.0f, srcY, 502.0f, 16.0f };
        Rectangle destRec = { 7.0f, 63.0f, 502.0f, 16.0f };
        DrawTexturePro(conveyorTex, srcRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    // 8. Draw conveyor belt plant cards on top of conveyor belt bar
    auto drawCard = [&](const std::string& plantType, Rectangle cRect) {
        Texture2D tex = res.GetTexture("WALLNUT");
        if (plantType == "GiantWallnut") {
            Texture2D giantTex = res.GetTexture("GIANTWALLNUT");
            if (giantTex.id == 0) giantTex = res.GetTexture("GiantWallnut");
            if (giantTex.id == 0) {
                std::string path = res.GetAssetPath("assets/PlantSeedPackets/GiantWallnut.png");
                res.LoadFile(path);
                giantTex = res.GetTexture("GIANTWALLNUT");
            }
            if (giantTex.id != 0) tex = giantTex;
        } else if (plantType == "ExplodeNut") {
            Texture2D explodeTex = res.GetTexture("EXPLODEONUT");
            if (explodeTex.id == 0) explodeTex = res.GetTexture("ExplodeONut");
            if (explodeTex.id == 0) {
                std::string path = res.GetAssetPath("assets/PlantSeedPackets/ExplodeONut.png");
                res.LoadFile(path);
                explodeTex = res.GetTexture("EXPLODEONUT");
            }
            if (explodeTex.id != 0) tex = explodeTex;
        }

        if (tex.id != 0) {
            DrawTexturePro(
                tex,
                { 0.0f, 0.0f, (float)tex.width, (float)tex.height },
                cRect,
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawRectangleRec(cRect, LIGHTGRAY);
            DrawText(plantType.c_str(), (int)cRect.x + 2, (int)cRect.y + 10, 10, BLACK);
        }
    };

    for (const auto& card : m_cards) {
        Rectangle cardRect = { card.x, 8.0f, 50.0f, 70.0f };
        drawCard(card.plantType, cardRect);
    }

    // 9. Draw held card attached directly under mouse cursor
    if (m_isHoldingCard) {
        Rectangle cursorCardRect = { mousePos.x - 25.0f, mousePos.y - 35.0f, 50.0f, 70.0f };
        drawCard(m_heldPlantType, cursorCardRect);
    }

    // 10. Draw Speed & Pause Controls
    drawSpeedControls();

    // 11. Draw Bottom-Right FlagMeter Progress Bar & "Wall-nut Bowling" Label
    drawProgressBar();

    // 12. Draw Win / Loss Overlays
    if (m_levelWon) {
        drawWinScreen();
    } else if (m_levelLost) {
        drawLoseScreen();
    }

    // 13. Draw "READY... SET... PLANT!" intro animation if active
    if (!m_readySetPlantDone) {
        m_readySetPlantAnim.Draw(400.0f, 300.0f, 1.0f);
    }

    // 14. Draw in-game pause menu dialog if open
    if (m_inGameMenu && m_inGameMenu->isOpen()) {
        m_inGameMenu->draw();
    }

    EndTextureMode();

    // 14. Draw targetScreen stretched to actual window dimensions
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

void BowlingLevel::drawSpeedControls() {
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
    Color speedColor = (m_gameSpeed > 1.0f) ? Color{ 255, 220, 40, 255 } : Color{ 230, 235, 245, 255 };

    if (m_brianneLoaded) {
        Rectangle textBounds = { speedBtn.x + 19.0f, speedBtn.y, speedBtn.width - 19.0f, speedBtn.height };
        Rectangle shadowBounds = { textBounds.x + 1.0f, textBounds.y + 1.0f, textBounds.width, textBounds.height };
        m_brianneFont.DrawTextCentered(speedBuf, shadowBounds, 1.0f, ColorAlpha(BLACK, 0.7f));
        m_brianneFont.DrawTextCentered(speedBuf, textBounds, 1.0f, speedColor);
    } else {
        DrawText(speedBuf, (int)(speedBtn.x + 24.0f), (int)(speedBtn.y + 4.0f), 17, speedColor);
    }
}

void BowlingLevel::drawLoseScreen() {
    // 1. Fade to dark red / black vignette
    float overlayAlpha = std::clamp(m_loseTimer * 1.8f, 0.0f, 0.88f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(Color{ 25, 0, 0, 255 }, overlayAlpha));

    // 2. Draw "THE ZOMBIES ATE YOUR BRAINS!" graphic with zoom-in ease-out
    Texture2D texZombiesWon = res.GetTexture("ZOMBIESWON");
    if (texZombiesWon.id == 0) texZombiesWon = res.GetTexture("ZombiesWon");
    if (texZombiesWon.id == 0) {
        std::string path = res.GetAssetPath("assets/reanim/ZombiesWon.jpg");
        if (FileExists(path.c_str())) {
            res.LoadFile(path);
            texZombiesWon = res.GetTexture("ZOMBIESWON");
            if (texZombiesWon.id == 0) texZombiesWon = res.GetTexture("ZombiesWon");
        }
    }

    if (texZombiesWon.id != 0) {
        float zoomProgress = std::clamp((m_loseTimer - 0.4f) * 1.8f, 0.0f, 1.0f);
        float easeZoom = zoomProgress * zoomProgress * (3.0f - 2.0f * zoomProgress);
        float scale = 0.40f + 0.45f * easeZoom;
        float w = (float)texZombiesWon.width * scale;
        float h = (float)texZombiesWon.height * scale;

        DrawTexturePro(
            texZombiesWon,
            { 0.0f, 0.0f, (float)texZombiesWon.width, (float)texZombiesWon.height },
            { 400.0f, 255.0f, w, h },
            { w / 2.0f, h / 2.0f },
            0.0f,
            ColorAlpha(WHITE, std::min(1.0f, (m_loseTimer - 0.3f) * 2.5f))
        );
    } else {
        m_font.DrawTextCentered("THE ZOMBIES ATE YOUR BRAINS!", { 2.0f, 222.0f, 800.0f, 50.0f }, 1.15f, ColorAlpha(BLACK, 0.9f));
        m_font.DrawTextCentered("THE ZOMBIES ATE YOUR BRAINS!", { 0.0f, 220.0f, 800.0f, 50.0f }, 1.15f, RED);
    }

    // 3. Prompt when defeat animation concludes
    if (m_loseTimer >= 1.6f) {
        float textAlpha = std::clamp((m_loseTimer - 1.6f) * 2.0f, 0.0f, 1.0f);
        Rectangle promptBox = { 230.0f, 515.0f, 340.0f, 44.0f };
        DrawRectangleRec(promptBox, ColorAlpha(BLACK, textAlpha * 0.75f));
        DrawRectangleLinesEx(promptBox, 2.0f, ColorAlpha(RED, textAlpha));
        if (m_brianneLoaded) {
            m_brianneFont.DrawTextCentered("Click anywhere to try again", { promptBox.x + 1.0f, promptBox.y + 1.0f, promptBox.width, promptBox.height }, 1.0f, ColorAlpha(BLACK, textAlpha * 0.8f));
            m_brianneFont.DrawTextCentered("Click anywhere to try again", promptBox, 1.0f, ColorAlpha(RAYWHITE, textAlpha));
        } else {
            DrawText("Click anywhere to try again", 270, 528, 18, ColorAlpha(RAYWHITE, textAlpha));
        }
    }
}

void BowlingLevel::drawWinScreen() {
    float overlayAlpha = std::clamp(m_winTimer * 1.5f, 0.0f, 0.75f);
    DrawRectangleRec({ 0, 0, 800, 600 }, ColorAlpha(BLACK, overlayAlpha));

    Texture2D texTrophy = res.GetTexture("TROPHY_HI_RES");
    if (texTrophy.id == 0) texTrophy = res.GetTexture("TROPHY");
    if (texTrophy.id == 0) texTrophy = res.GetTexture("Trophy_hi_res");

    float centerX = 400.0f;
    float centerY = m_awardY;

    // Draw rotating sun rays behind trophy
    Texture2D texRaysImg = res.GetTexture("AWARDRAYS");
    if (texRaysImg.id == 0) texRaysImg = res.GetTexture("AwardRays");
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
        Rectangle bannerRect = { 175.0f, 345.0f, 450.0f, 180.0f };
        DrawRectangleRec(bannerRect, ColorAlpha(Color{ 20, 25, 40, 255 }, bannerAlpha * 0.92f));
        DrawRectangleLinesEx(bannerRect, 3.0f, ColorAlpha(GOLD, bannerAlpha));

        // 1. Big Title: "LEVEL COMPLETED!" with HouseofTerror28
        Rectangle shadowTitle = { bannerRect.x + 2.0f, bannerRect.y + 14.0f, bannerRect.width, 36.0f };
        Rectangle textTitle = { bannerRect.x, bannerRect.y + 12.0f, bannerRect.width, 36.0f };
        m_font.DrawTextCentered("LEVEL COMPLETED!", shadowTitle, 1.0f, ColorAlpha(BLACK, 0.8f * bannerAlpha));
        m_font.DrawTextCentered("LEVEL COMPLETED!", textTitle, 1.0f, ColorAlpha(GOLD, bannerAlpha));

        // 2. Subtitle: with BrianneTod16
        Rectangle subRect = { bannerRect.x, bannerRect.y + 54.0f, bannerRect.width, 22.0f };
        if (m_brianneLoaded) {
            m_brianneFont.DrawTextCentered("You bowled through the zombie horde!", subRect, 0.95f, ColorAlpha(WHITE, bannerAlpha));
        } else {
            DrawText("You bowled through the zombie horde!", 235, 400, 17, ColorAlpha(WHITE, bannerAlpha));
        }

        // 3. Cash Reward Line: with BrianneTod16 in bright coin gold
        std::string rewardStr = "+2,000 Coins Earned!";
        Rectangle rewardRect = { bannerRect.x, bannerRect.y + 84.0f, bannerRect.width, 24.0f };
        if (m_brianneLoaded) {
            m_brianneFont.DrawTextCentered(rewardStr.c_str(), { rewardRect.x + 1.0f, rewardRect.y + 1.0f, rewardRect.width, rewardRect.height }, 1.15f, ColorAlpha(BLACK, 0.8f * bannerAlpha));
            m_brianneFont.DrawTextCentered(rewardStr.c_str(), rewardRect, 1.15f, ColorAlpha(Color{ 255, 220, 50, 255 }, bannerAlpha));
        } else {
            DrawText(rewardStr.c_str(), 320, 430, 19, ColorAlpha(GOLD, bannerAlpha));
        }

        // 4. Continue prompt
        Rectangle promptRect = { bannerRect.x, bannerRect.y + 128.0f, bannerRect.width, 22.0f };
        if (m_brianneLoaded) {
            m_brianneFont.DrawTextCentered("Click anywhere or press ENTER to continue", promptRect, 0.85f, ColorAlpha(Color{ 200, 200, 200, 255 }, bannerAlpha));
        } else {
            DrawText("Click anywhere or press ENTER to continue", 230, 474, 16, ColorAlpha(LIGHTGRAY, bannerAlpha));
        }
    }
}

void BowlingLevel::drawProgressBar() {
    Texture2D texMeter = res.GetTexture("FLAGMETER");
    if (texMeter.id == 0) {
        std::string path = res.GetAssetPath("assets/images/FlagMeter.png");
        res.LoadFile(path);
        texMeter = res.GetTexture("FLAGMETER");
    }

    Texture2D texBadge = res.GetTexture("FLAGMETERLEVELPROGRESS");
    if (texBadge.id == 0) {
        std::string path = res.GetAssetPath("assets/images/FlagMeterLevelProgress.png");
        res.LoadFile(path);
        texBadge = res.GetTexture("FLAGMETERLEVELPROGRESS");
    }

    Texture2D texParts = res.GetTexture("FLAGMETERPARTS");
    if (texParts.id == 0) {
        std::string path = res.GetAssetPath("assets/images/FlagMeterParts.png");
        res.LoadFile(path);
        texParts = res.GetTexture("FLAGMETERPARTS");
    }

    if (texMeter.id == 0) return;

    // Progress computation (0.0 to 1.0)
    float waveProgress = 0.0f;
    if (m_currentWave == 0) {
        float waveFraction = std::clamp((5.0f - m_waveTimer) / 5.0f, 0.0f, 1.0f);
        waveProgress = (0.0f + waveFraction) / (float)std::max(1, m_maxWaves);
    } else if (m_currentWave < m_maxWaves) {
        float waveFraction = std::clamp((22.0f - m_waveTimer) / 22.0f, 0.0f, 1.0f);
        waveProgress = ((float)m_currentWave + waveFraction) / (float)m_maxWaves;
    } else {
        waveProgress = 1.0f;
    }
    waveProgress = std::clamp(waveProgress, 0.0f, 1.0f);

    float barX = 625.0f;
    float barY = 572.0f;

    // 1. Draw "Wall-nut Bowling" label to the left with House of Terror font
    float labelScale = 0.58f;
    int textW = m_font.MeasureText("Wall-nut Bowling", labelScale);
    float textX = barX - (float)textW - 10.0f;
    float textY = barY - 6.0f;

    // Drop shadow
    m_font.DrawText("Wall-nut Bowling", textX + 2.0f, textY + 2.0f, labelScale, Color{ 0, 0, 0, 255 });
    // Golden yellow text
    m_font.DrawText("Wall-nut Bowling", textX, textY, labelScale, Color{ 235, 200, 45, 255 });

    // 2. Draw Progress Bar Frame (0, 0, 158, 25)
    Rectangle srcFrame = { 0.0f, 0.0f, 158.0f, 25.0f };
    DrawTextureRec(texMeter, srcFrame, { barX, barY }, WHITE);

    // 3. Draw Green Progress Fill (filling from right to left)
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

    // 5. Draw Red Flag at final wave milestone (1.0f)
    if (texParts.id != 0) {
        float frac = 1.0f;
        float flagX = barX + 155.0f - maxFillWidth * frac - 8.0f;
        flagX = std::max(barX + 6.0f, flagX);

        // Flag pole (Part 1: x = 25..50)
        Rectangle srcPole = { 25.0f, 0.0f, 25.0f, 25.0f };
        DrawTextureRec(texParts, srcPole, { flagX, barY - 2.0f }, WHITE);

        // Red flag (Part 2: x = 50..75) - raised when near/reached final wave fraction
        float flagOffsetY = (waveProgress >= frac - 0.05f) ? -6.0f : -2.0f;
        Rectangle srcFlag = { 50.0f, 0.0f, 25.0f, 25.0f };
        DrawTextureRec(texParts, srcFlag, { flagX, barY + flagOffsetY }, WHITE);

        // 6. Draw Zombie Head Slider Marker (Part 0: x = 0..25)
        float headX = barX + 155.0f - currentFillWidth - 11.0f;
        headX = std::clamp(headX, barX + 6.0f, barX + 144.0f);
        Rectangle srcHead = { 0.0f, 0.0f, 25.0f, 25.0f };
        DrawTextureRec(texParts, srcHead, { headX, barY - 2.0f }, WHITE);
    }
}

void BowlingLevel::run() {
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

        if (m_exitToMainMenu || (m_levelWon && IsKeyPressed(KEY_ENTER)) || (m_levelWon && IsKeyPressed(KEY_ESCAPE))) {
            break;
        }
    }
}
