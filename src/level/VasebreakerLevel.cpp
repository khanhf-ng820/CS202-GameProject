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

    // Preload textures
    std::string potPath = res.GetAssetPath("assets/images/Scary_Pot.png");
    res.LoadFile(potPath);
    std::string chunksPath = res.GetAssetPath("assets/particles/vase_chunks.png");
    res.LoadFile(chunksPath);

    // Populate initial vases
    spawnVases();
}

void VasebreakerLevel::spawnVases() {
    // Clear all cells
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_vases[r][c].reset();
        }
    }

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

    m_vases[row][col]->destroy();

    // Play shattering sound effect
    std::string shatterSound = res.GetAssetPath("assets/sounds/vase_breaking.ogg");
    AudioManager::GetInstance().PlaySoundEffect(shatterSound);

    // Determine shard texture row (0 for Brown Mystery Vase, 1 for Green Leaf Vase)
    int shardRow = (m_vases[row][col]->getType() == VaseType::Green) ? 1 : 0;
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

    // Left-click to trigger mallet strike swing animation
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_isSwinging = true;
        m_malletAnim.SetAnimation("anim_open_pot");
        m_malletAnim.SetFrame(9.0f); // Start of strike swing
        m_malletAnim.SetSpeed(2.5f); // Fast responsive swing
        m_malletAnim.SetPaused(false);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/swing.ogg"));

        // Only target ONE vase at a time that is targetable (Intact)
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

    // Update active zombies
    for (auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->update(dt);
            // Check loss condition: living zombie advances past the lawn limit into the house
            if (!z->isDead() && z->getX() < 160.0f) {
                m_levelLost = true;
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
        DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
    }

    // 3. Draw active intact & pending vases
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_vases[r][c] && !m_vases[r][c]->isDestroyed()) {
                m_vases[r][c]->draw(res);
            }
        }
    }

    // 4. Draw ceramic shards
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

    // 5. Draw active zombies
    for (const auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->draw();
        }
    }

    // 6. Draw Game Over / Loss overlay if triggered
    if (m_levelLost) {
        DrawRectangleRec({ 200, 200, 400, 200 }, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx({ 200, 200, 400, 200 }, 3.0f, RED);
        DrawText("THE ZOMBIES ATE YOUR BRAINS!", 215, 240, 22, RED);
        DrawText("Game Over!", 350, 290, 20, WHITE);
        DrawText("Press ESC to return", 300, 340, 16, LIGHTGRAY);
    }

    // 7. Draw Mallet cursor at virtual mouse position (Option 1A: -42.0f, -6.0f)
    m_malletAnim.Draw(mousePos.x - 42.0f, mousePos.y - 6.0f, 1.0f);

    // 8. Draw debug red dot showing exact virtual cursor coordinates
    DrawCircle((int)mousePos.x, (int)mousePos.y, 4.0f, RED);
    DrawCircleLines((int)mousePos.x, (int)mousePos.y, 4.0f, WHITE);

    EndTextureMode();

    // 9. Draw targetScreen stretched to actual window dimensions
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
