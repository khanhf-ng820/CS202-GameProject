#include "BowlingLevel.h"
#include "UIHelpers.h"
#include "AudioManager.h"

BowlingLevel::BowlingLevel(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen) {
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
    float spawnX = 700.0f;

    auto laneY = [](int row) -> float {
        return 50.0f + row * 100.0f;
    };

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(3)));
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 5) {
        // Final wave!
        m_finalWaveAnnounced = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(4)));
    }
}

void BowlingLevel::update(float dt) {
    if (m_levelWon || m_levelLost) return;

    // 0. Wave spawn timer
    if (m_currentWave < m_maxWaves) {
        m_waveTimer -= dt;
        if (m_waveTimer <= 0.0f) {
            spawnNextWave();
            m_waveTimer = 22.0f;
        }
    }

    // 1. Advance conveyor belt animation frame (6 rows of 16px each in ConveyorBelt.png)
    m_animTimer += dt;
    float frameDuration = 0.08f; // ~12.5 FPS animation speed
    if (m_animTimer >= frameDuration) {
        m_animTimer -= frameDuration;
        m_currentFrame = (m_currentFrame + 1) % 6;
    }

    // 2. Spawn a Wall-nut card every 3 seconds at right end of conveyor belt (spawnX = 459.0f)
    float spawnX = 459.0f;
    float leftMinX = 9.0f;
    float cardW = 50.0f;

    m_cardSpawnTimer += dt;
    if (m_cardSpawnTimer >= 3.0f) {
        // Only spawn a new card if the conveyor belt has room (last card has moved left of spawn position)
        if (m_cards.empty() || m_cards.back().x < spawnX) {
            int roll = GetRandomValue(1, 100);
            std::string plantType = "Wallnut";
            if (roll <= 20) {
                plantType = "GiantWallnut";
            } else if (roll <= 40) {
                plantType = "ExplodeNut";
            }
            m_cards.push_back({ spawnX, plantType });
            m_cardSpawnTimer = 0.0f;
        } else {
            // Conveyor belt is full; cap timer at 3.0s so a card spawns immediately when space opens up
            m_cardSpawnTimer = 3.0f;
        }
    }

    // 3. Move cards leftward toward left end of conveyor belt (leftMinX = 9.0f) with 0 spacing
    float cardSpeed = 60.0f; // 60 px/s
    for (size_t i = 0; i < m_cards.size(); ++i) {
        float targetX = (i == 0) ? leftMinX : (m_cards[i - 1].x + cardW);
        if (m_cards[i].x > targetX) {
            m_cards[i].x -= cardSpeed * dt;
            if (m_cards[i].x < targetX) {
                m_cards[i].x = targetX;
            }
        } else if (m_cards[i].x < targetX) {
            m_cards[i].x = targetX;
        }
    }

    Vector2 mousePos = GetVirtualMousePosition();

    // 4. Handle Debug toggle button click (700, 10, 90, 30)
    Rectangle debugBtnRect = { 700.0f, 10.0f, 90.0f, 30.0f };
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, debugBtnRect)) {
        m_showDebug = !m_showDebug;
    }

    // 5. Handle right-click on grid to spawn PoleVaultingZombie at right of lane (x = 700.0f, y = 50.0f + r * 100.0f)
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        int r, c;
        if (getGridCell(mousePos, r, c)) {
            m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, 700.0f, 50.0f + r * 100.0f));
        }
    }

    // 5. Handle card pickup from conveyor belt (when not currently holding a card)
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

    // 6. Handle plant placement on lawn grid (when holding a card)
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

    // 7. Update rolling bowling nuts & handle zombie collisions + boundary bouncing
    for (auto& nut : m_bowlingNuts) {
        nut->update(dt, m_zombies, m_hitDebugTimers, res);
    }

    // Update hit debug timers (decrement and cleanup)
    for (auto& item : m_hitDebugTimers) {
        item.second -= dt;
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

    // 8. Update zombies (right to left movement) and check loss condition
    for (auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->update(dt);
            // Check loss condition: Living Zombie reaches house (x < 160.0f)
            if (!z->isDead() && z->getX() < 160.0f) {
                m_levelLost = true;
            }
        }
    }

    // Clean up finished zombies
    m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
        [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }), m_zombies.end());

    // Check win condition: All waves spawned and all zombies defeated
    if (m_currentWave >= m_maxWaves) {
        bool anyZombieAlive = false;
        for (const auto& z : m_zombies) {
            if (!z->isDead()) {
                anyZombieAlive = true;
                break;
            }
        }
        if (!anyZombieAlive) {
            m_levelWon = true;
        }
    }

    // 9. Update placed plants (if any)
    std::vector<Projectile> dummyProjectiles;
    std::vector<SunItem> dummySuns;
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c]) {
                m_grid[r][c]->update(dt, dummyProjectiles, dummySuns);
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

    // 3. Draw placed plants on lawn grid
    for (int r = 0; r < 5; ++r) {
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

    // Draw Debug toggle button in top right UI area (700, 10, 90, 30)
    Rectangle debugBtnRect = { 700.0f, 10.0f, 90.0f, 30.0f };
    DrawRectangleRec(debugBtnRect, m_showDebug ? DARKGREEN : DARKGRAY);
    DrawRectangleLinesEx(debugBtnRect, 2.0f, WHITE);
    DrawText(m_showDebug ? "Debug: ON" : "Debug: OFF", 708, 17, 14, WHITE);

    // 6. Draw hover highlight cell on front lawn grid
    Vector2 mousePos = GetVirtualMousePosition();
    int hoverRow, hoverCol;
    if (getGridCell(mousePos, hoverRow, hoverCol)) {
        float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
        float cellY = 80.0f + hoverRow * 100.0f;
        float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
        float cellH = 100.0f;

        if (m_isHoldingCard) {
            // ONLY ALLOW placement on tiles to the left of the red bowling stripe (columns 0, 1, 2)
            // Tiles are immediately available for placement
            if (hoverCol <= 2) {
                DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
            } else {
                DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(RED, 0.6f));
            }
        } else {
            DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
        }
    }

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

    // 10. Draw Win / Loss Overlays (identical to Level 1)
    if (m_levelWon) {
        DrawRectangleRec({ 200, 200, 400, 200 }, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx({ 200, 200, 400, 200 }, 3.0f, GOLD);
        DrawText("LEVEL COMPLETED!", 260, 240, 28, GOLD);
        DrawText("You defeated all zombies!", 270, 290, 18, WHITE);
        DrawText("Press ESC to return", 300, 340, 16, LIGHTGRAY);
    } else if (m_levelLost) {
        DrawRectangleRec({ 200, 200, 400, 200 }, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx({ 200, 200, 400, 200 }, 3.0f, RED);
        DrawText("THE ZOMBIES ATE YOUR BRAINS!", 215, 240, 22, RED);
        DrawText("Game Over!", 350, 290, 20, WHITE);
        DrawText("Press ESC to return", 300, 340, 16, LIGHTGRAY);
    }

    EndTextureMode();

    // 11. Draw targetScreen stretched to actual window dimensions
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

        if (IsKeyPressed(KEY_ESCAPE) || (m_levelWon && IsKeyPressed(KEY_ENTER))) {
            break;
        }
    }
}
