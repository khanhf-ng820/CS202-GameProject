#include "VasebreakerLevel.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <algorithm>
#include <cmath>

VasebreakerLevel::VasebreakerLevel(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen) {
    // Load and initialize the mallet cursor reanimation
    std::string hammerPath = res.GetAssetPath("assets/reanim/Hammer.reanim");
    ReanimDefinition hammerDef = res.LoadReanim(hammerPath);
    m_malletAnim.SetResources(hammerDef, res);
    m_malletAnim.SetAnimation("anim_open_pot");
    m_malletAnim.SetFrame(14.0f); // Resting upright angle
    m_malletAnim.SetPaused(true);

    // Initialize plant placement preview reanimation
    std::string peaPath = res.GetAssetPath("assets/reanim/PeaShooter.reanim");
    ReanimDefinition peaDef = res.LoadReanim(peaPath);
    m_previewPlantAnim.SetResources(peaDef, res);
    m_previewPlantAnim.SetBaseAnimation("anim_idle");
    m_previewPlantAnim.SetAnimation("anim_head_idle");

    // Preload textures
    std::string potPath = res.GetAssetPath("assets/images/Scary_Pot.png");
    res.LoadFile(potPath);
    std::string chunksPath = res.GetAssetPath("assets/particles/vase_chunks.png");
    res.LoadFile(chunksPath);
    std::string repeaterCardPath = res.GetAssetPath("assets/PlantSeedPackets/REPEATER.png");
    res.LoadFile(repeaterCardPath);

    // Populate initial vases
    spawnVases();
}

void VasebreakerLevel::spawnVases() {
    // Clear all cells
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_vases[r][c].reset();
            m_plants[r][c].reset();
        }
    }
    m_droppedPackets.clear();
    m_selectedPacketIndex = -1;
    m_projectiles.clear();
    m_shards.clear();

    // Populate columns 6, 7, 8 across all 5 rows
    for (int r = 0; r < 5; ++r) {
        for (int c = 6; c < 9; ++c) {
            float cellX = 140.0f + (c == 0 ? 0.0f : 80.0f + (c - 1) * 70.0f);
            float cellY = 80.0f + r * 100.0f;
            float cellW = (c == 0) ? 80.0f : 70.0f;
            float cellH = 100.0f;
            float centerX = cellX + cellW / 2.0f;
            float centerY = cellY + cellH / 2.0f;
            float vaseX = centerX - 40.0f;
            float vaseY = centerY - 50.0f;

            if ((r == 1 && c == 7) || (r == 3 && c == 7)) {
                m_vases[r][c] = std::make_unique<GreenVase>(r, c, vaseX, vaseY);
            } else {
                m_vases[r][c] = std::make_unique<BrownVase>(r, c, vaseX, vaseY);
            }
        }
    }
}

void VasebreakerLevel::breakVase(int row, int col) {
    if (row < 0 || row >= 5 || col < 0 || col >= 9 || !m_vases[row][col]) return;

    VaseType vType = m_vases[row][col]->getType();
    m_vases[row][col]->destroy();

    // Play shattering sound effect
    std::string shatterSound = res.GetAssetPath("assets/sounds/vase_breaking.ogg");
    AudioManager::GetInstance().PlaySoundEffect(shatterSound);

    // Determine shard texture row (0 for Brown Mystery Vase, 1 for Green Leaf Vase)
    int shardRow = (vType == VaseType::Green) ? 1 : 0;
    float cx = m_vases[row][col]->getX() + 40.0f;
    float cy = m_vases[row][col]->getY() + 50.0f;
    float groundY = cy + 35.0f;

    // Spawn 16 ceramic shards
    for (int i = 0; i < 16; ++i) {
        VaseShard shard;
        shard.x = cx + (float)GetRandomValue(-15, 15);
        shard.y = cy + (float)GetRandomValue(-25, 20);
        float angleDeg = (float)GetRandomValue(200, 340); // upward fan
        float speed = (float)GetRandomValue(120, 280);
        shard.vx = cosf(angleDeg * DEG2RAD) * speed;
        shard.vy = sinf(angleDeg * DEG2RAD) * speed;
        shard.rotation = (float)GetRandomValue(0, 360);
        shard.rotSpeed = (float)GetRandomValue(-720, 720);
        shard.groundY = groundY + (float)GetRandomValue(-8, 8);
        shard.maxLifetime = (float)GetRandomValue(70, 95) / 100.0f; // 0.70s to 0.95s
        shard.lifetime = shard.maxLifetime;
        shard.frameCol = GetRandomValue(0, 8);
        shard.frameRow = shardRow;
        m_shards.push_back(shard);
    }

    // If a Green Vase is destroyed, spawn and drop a "Repeater" plant seed packet card
    if (vType == VaseType::Green) {
        DroppedSeedPacket packet;
        packet.width = 50.0f;
        packet.height = 70.0f;
        packet.x = cx - packet.width / 2.0f;
        packet.startY = cy - 40.0f;
        packet.y = packet.startY;
        packet.groundY = cy - packet.height / 2.0f + 10.0f;
        packet.vy = -120.0f; // slight upward pop
        packet.plantType = "Repeater";
        m_droppedPackets.push_back(packet);
    }
}

bool VasebreakerLevel::getGridCell(Vector2 mousePos, int& outRow, int& outCol) const {
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

void VasebreakerLevel::update(float dt) {
    if (m_levelLost || m_levelWon) return;

    Vector2 mousePos = GetVirtualMousePosition();

    // Update plant preview animation
    m_previewPlantAnim.Update(dt);

    // Update dropped seed packet cards
    for (auto& packet : m_droppedPackets) {
        packet.update(dt);
    }

    // Left-click interaction handling
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool handledCardClick = false;

        // 1. Check if clicking on any dropped seed packet card
        for (int i = 0; i < (int)m_droppedPackets.size(); ++i) {
            if (m_droppedPackets[i].isClicked(mousePos)) {
                if (m_selectedPacketIndex == i) {
                    // Click selected card again -> Deselect card and return to mallet mode
                    m_selectedPacketIndex = -1;
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/tap.ogg"));
                } else {
                    // Select card for planting
                    m_selectedPacketIndex = i;
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/seedlift.ogg"));
                }
                handledCardClick = true;
                break;
            }
        }

        if (!handledCardClick) {
            if (m_selectedPacketIndex >= 0) {
                // 2. A card is selected: try placing plant on an empty lawn tile
                int plantRow, plantCol;
                if (getGridCell(mousePos, plantRow, plantCol)) {
                    bool tileEmpty = (!m_plants[plantRow][plantCol]) &&
                                     (!m_vases[plantRow][plantCol] || m_vases[plantRow][plantCol]->isDestroyed());
                    if (tileEmpty) {
                        float cellX = 140.0f + (plantCol == 0 ? 0.0f : 80.0f + (plantCol - 1) * 70.0f);
                        float cellY = 80.0f + plantRow * 100.0f;
                        m_plants[plantRow][plantCol] = std::make_unique<Repeater>(res, (int)cellX, (int)cellY);
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/plant.ogg"));

                        // Consume and remove the dropped card
                        if (m_selectedPacketIndex >= 0 && m_selectedPacketIndex < (int)m_droppedPackets.size()) {
                            m_droppedPackets.erase(m_droppedPackets.begin() + m_selectedPacketIndex);
                        }
                        m_selectedPacketIndex = -1;
                    }
                    // If tile is occupied by an intact vase or existing plant, ignore placement and keep card selected
                }
            } else {
                // 3. No card selected: standard mallet swing & vase smashing
                m_isSwinging = true;
                m_malletAnim.SetAnimation("anim_open_pot");
                m_malletAnim.SetFrame(9.0f); // Start of strike swing
                m_malletAnim.SetSpeed(2.5f); // Fast responsive swing
                m_malletAnim.SetPaused(false);
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/swing.ogg"));

                // Target at most ONE vase that is targetable (Intact)
                int clickRow, clickCol;
                if (getGridCell(mousePos, clickRow, clickCol)) {
                    if (m_vases[clickRow][clickCol] && m_vases[clickRow][clickCol]->isTargetable() && m_pendingVaseRow == -1) {
                        m_vases[clickRow][clickCol]->setPendingBreak();
                        m_pendingVaseRow = clickRow;
                        m_pendingVaseCol = clickCol;
                        m_pendingVaseTimer = 0.15f; // Mallet strike impact delay
                    }
                }
            }
        }
    }

    // Process mallet swing animation
    if (m_isSwinging) {
        m_malletAnim.Update(dt);
        if (m_malletAnim.GetCurrentFrame() >= m_malletAnim.GetEndFrame() - 1 || m_malletAnim.GetCurrentFrame() >= 16) {
            m_isSwinging = false;
            m_malletAnim.SetFrame(14.0f); // Return to resting upright pose
            m_malletAnim.SetPaused(true);
        }
    }

    // Process pending vase strike impact
    if (m_pendingVaseTimer > 0.0f) {
        m_pendingVaseTimer -= dt;
        if (m_pendingVaseTimer <= 0.0f && m_pendingVaseRow >= 0) {
            breakVase(m_pendingVaseRow, m_pendingVaseCol);
            m_pendingVaseRow = -1;
            m_pendingVaseCol = -1;
            m_pendingVaseTimer = 0.0f;
        }
    }

    // Update active placed plants & targeting logic
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_plants[r][c] && !m_plants[r][c]->isDead()) {
                bool hasZombieInRow = false;
                float plantX = (float)m_plants[r][c]->getX();

                for (const auto& z : m_zombies) {
                    if (!z->isDead()) {
                        int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                        if (zRow == r && z->getX() >= plantX - 20.0f && z->getX() <= 800.0f) {
                            hasZombieInRow = true;
                            break;
                        }
                    }
                }

                if (hasZombieInRow) {
                    if (m_plants[r][c]->getAnim().GetCurrentAnimName() != "anim_shooting") {
                        m_plants[r][c]->SetAnimation("anim_shooting");
                    }
                } else {
                    if (m_plants[r][c]->getAnim().GetCurrentAnimName() == "anim_shooting") {
                        m_plants[r][c]->SetAnimation("anim_head_idle");
                    }
                }

                m_plants[r][c]->update(dt, m_projectiles, m_suns);
            }
        }
    }

    // Clean up dead plants
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_plants[r][c] && m_plants[r][c]->isDead()) {
                m_plants[r][c].reset();
            }
        }
    }

    // Update projectiles & projectile-zombie collisions
    for (auto& p : m_projectiles) {
        p.update(dt);
        if (!p.isActive()) continue;

        for (auto& z : m_zombies) {
            if (!z->isDead()) {
                int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                int pRow = (int)((p.getY() - 80.0f + 50.0f) / 100.0f);
                if (zRow == pRow && fabsf(p.getX() - (z->getX() + 40.0f)) < 30.0f) {
                    z->takeDamage(20.0f);
                    p.deactivate();
                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/splat.ogg"));
                    break;
                }
            }
        }
    }

    // Clean up inactive projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }),
        m_projectiles.end()
    );

    // Update active shards
    for (auto& shard : m_shards) {
        shard.lifetime -= dt;
        shard.vy += 500.0f * dt; // Gravity
        shard.x += shard.vx * dt;
        shard.y += shard.vy * dt;
        shard.rotation += shard.rotSpeed * dt;

        // Ground bounce
        if (shard.y >= shard.groundY && shard.vy > 0.0f) {
            shard.y = shard.groundY;
            shard.vy = -shard.vy * 0.4f; // bounce restitution
            shard.vx *= 0.7f;            // ground friction
            shard.rotSpeed *= 0.6f;
        }
    }

    // Remove expired shards
    m_shards.erase(
        std::remove_if(m_shards.begin(), m_shards.end(),
            [](const VaseShard& s) { return s.lifetime <= 0.0f; }),
        m_shards.end()
    );

    // Right-click a tile in a lane to spawn a ZombieNormal
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        int row, col;
        if (getGridCell(mousePos, row, col)) {
            m_zombies.push_back(std::make_unique<ZombieNormal>(res, 700.0f, 45.0f + row * 100.0f));
        }
    }

    // Update active zombies and zombie-eating-plant interactions
    for (auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->update(dt);
            if (!z->isDead()) {
                int zRow = (int)((z->getY() - 45.0f + 50.0f) / 100.0f);
                bool foundPlantToEat = false;

                if (zRow >= 0 && zRow < 5) {
                    for (int c = 0; c < 9; ++c) {
                        Plant* p = m_plants[zRow][c].get();
                        if (p && !p->isDead()) {
                            float plantX = (float)p->getX();
                            if (z->getX() >= plantX - 20.0f && z->getX() <= plantX + 45.0f) {
                                foundPlantToEat = true;
                                z->setEating(true);
                                if (z->getAnim().GetCurrentAnimName() != "anim_eat") {
                                    z->getAnim().SetAnimation("anim_eat");
                                }

                                p->takeDamage((float)z->getDamage() * dt);

                                // Periodic chomp sound effect while eating
                                z->addEatTimer(dt);
                                if (z->getEatTimer() >= 0.35f) {
                                    z->resetEatTimer();
                                    int chompChoice = GetRandomValue(0, 1);
                                    std::string chompSfx = (chompChoice == 0) ? "assets/sounds/chomp.ogg" : "assets/sounds/chomp2.ogg";
                                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(chompSfx));
                                }

                                if (p->isDead()) {
                                    m_plants[zRow][c].reset();
                                    z->setEating(false);
                                    z->resetEatTimer();
                                    z->getAnim().SetAnimation("anim_walk");
                                }
                                break;
                            }
                        }
                    }
                }

                if (!foundPlantToEat) {
                    if (z->isEating() || z->getAnim().GetCurrentAnimName() == "anim_eat") {
                        z->setEating(false);
                        z->resetEatTimer();
                        z->getAnim().SetAnimation("anim_walk");
                    }
                }

                // Check loss condition: living zombie reaches house limit
                if (z->getX() < 160.0f) {
                    m_levelLost = true;
                }
            }
        }
    }

    // Clean up finished zombies
    m_zombies.erase(
        std::remove_if(m_zombies.begin(), m_zombies.end(),
            [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }),
        m_zombies.end()
    );
}

void VasebreakerLevel::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Nighttime Front Lawn Background (background2.jpg)
    Texture2D bgTex = res.GetTexture("BACKGROUND2");
    if (bgTex.id == 0) bgTex = res.GetTexture("background2");
    if (bgTex.id == 0) {
        std::string bgPath = res.GetAssetPath("assets/images/background2.jpg");
        res.LoadFile(bgPath);
        bgTex = res.GetTexture("BACKGROUND2");
    }

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
        // Fallback grid
        for (int y = 0; y < 600; y += 80) {
            for (int x = 0; x < 800; x += 80) {
                DrawRectangle(x, y, 80, 80, ((x / 80 + y / 80) % 2 == 0) ? DARKBLUE : DARKPURPLE);
            }
        }
    }

    // 2. Draw hover highlight on lawn grid
    Vector2 mousePos = GetVirtualMousePosition();
    int hoverRow, hoverCol;
    if (getGridCell(mousePos, hoverRow, hoverCol)) {
        float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
        float cellY = 80.0f + hoverRow * 100.0f;
        float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
        float cellH = 100.0f;

        if (m_selectedPacketIndex >= 0) {
            bool tileEmpty = (!m_plants[hoverRow][hoverCol]) &&
                             (!m_vases[hoverRow][hoverCol] || m_vases[hoverRow][hoverCol]->isDestroyed());
            Color outlineColor = tileEmpty ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(RED, 0.6f);
            DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, outlineColor);
        } else {
            DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
        }
    }

    // 3. Draw active placed plants
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_plants[r][c] && !m_plants[r][c]->isDead()) {
                m_plants[r][c]->draw();
            }
        }
    }

    // 4. Draw active intact & pending vases
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_vases[r][c] && !m_vases[r][c]->isDestroyed()) {
                m_vases[r][c]->draw(res);
            }
        }
    }

    // 5. Draw dropped seed packet cards (z-index in front of vases)
    for (int i = 0; i < (int)m_droppedPackets.size(); ++i) {
        m_droppedPackets[i].draw(res, m_selectedPacketIndex == i);
    }

    // 6. Draw ceramic shards
    Texture2D chunksTex = res.GetTexture("VASE_CHUNKS");
    if (chunksTex.id == 0) chunksTex = res.GetTexture("vase_chunks");
    if (chunksTex.id == 0) {
        std::string chunksPath = res.GetAssetPath("assets/particles/vase_chunks.png");
        res.LoadFile(chunksPath);
        chunksTex = res.GetTexture("VASE_CHUNKS");
        if (chunksTex.id == 0) chunksTex = res.GetTexture("vase_chunks");
    }

    if (chunksTex.id != 0) {
        for (const auto& shard : m_shards) {
            float alpha = 1.0f;
            if (shard.lifetime < 0.3f) {
                alpha = shard.lifetime / 0.3f;
            }
            Color tint = ColorAlpha(WHITE, alpha);

            Rectangle srcRec = {
                shard.frameCol * 32.0f,
                shard.frameRow * 32.0f,
                32.0f,
                32.0f
            };
            Rectangle destRec = {
                shard.x,
                shard.y,
                28.0f,
                28.0f
            };
            Vector2 origin = { 14.0f, 14.0f };
            DrawTexturePro(chunksTex, srcRec, destRec, origin, shard.rotation, tint);
        }
    }

    // 7. Draw active zombies
    for (const auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->draw();
        }
    }

    // 8. Draw active projectiles
    for (const auto& p : m_projectiles) {
        p.draw();
    }

    // 9. Draw Game Over / Loss overlay if triggered
    if (m_levelLost) {
        DrawRectangleRec({ 200, 200, 400, 200 }, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx({ 200, 200, 400, 200 }, 3.0f, RED);
        DrawText("THE ZOMBIES ATE YOUR BRAINS!", 215, 240, 22, RED);
        DrawText("Game Over!", 350, 290, 20, WHITE);
        DrawText("Press ESC to return", 300, 340, 16, LIGHTGRAY);
    }

    // 10. Draw Cursor
    if (m_selectedPacketIndex >= 0) {
        // Translucent plant preview following mouse cursor
        m_previewPlantAnim.Draw(mousePos.x - 30.0f, mousePos.y - 40.0f, 1.0f, ColorAlpha(WHITE, 0.65f));
    } else {
        // Wooden Mallet cursor at virtual mouse position (Option 1A: -42.0f, -6.0f)
        m_malletAnim.Draw(mousePos.x - 42.0f, mousePos.y - 6.0f, 1.0f);
    }

    // 11. Draw debug red dot showing exact virtual cursor coordinates
    DrawCircle((int)mousePos.x, (int)mousePos.y, 4.0f, RED);
    DrawCircleLines((int)mousePos.x, (int)mousePos.y, 4.0f, WHITE);

    EndTextureMode();

    // 12. Draw targetScreen stretched to actual window dimensions
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

void VasebreakerLevel::run() {
    SetUIInteractionEnabled(true);
    HideCursor();
    AudioManager::GetInstance().PlayMusic(MusicTrack::Vasebreaker);

    while (!WindowShouldClose()) {
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        float dt = GetFrameTime();
        AudioManager::GetInstance().Update();
        update(dt);
        draw();

        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
    }

    ShowCursor();
}
