#include "Level1.h"
#include "UIHelpers.h"
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
#include "ZombieNormal.h"
#include "FlagZombie.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>

Level1::Level1(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen), m_seedBank(150),
      m_skySunTimer(0.0f), m_waveTimer(12.0f), m_currentWave(0),
      m_maxWaves(5), m_levelWon(false), m_levelLost(false),
      m_finalWaveAnnounced(false) 
{
    // Clear grid
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            m_grid[r][c] = nullptr;
        }
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
    }
}

void Level1::spawnNextWave() {
    m_currentWave++;
    float spawnX = 700.0f;

    auto laneY = [](int row) -> float {
        return 65.0f + row * 100.0f;
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
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 5) {
        // Final wave!
        m_finalWaveAnnounced = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(4)));
    }
}

void Level1::updateCollisions(float dt) {
    // 1. Projectiles vs Zombies
    for (auto& p : m_projectiles) {
        if (!p.isActive()) continue;
        for (auto& z : m_zombies) {
            if (z->isDead()) continue;
            // Check Y lane match (within 50 pixels) and X collision
            if (std::abs(p.getY() - (z->getY() + 40.0f)) < 55.0f) {
                if (p.getX() >= z->getX() + 10.0f && p.getX() <= z->getX() + 80.0f) {
                    z->takeDamage(p.getDamage());
                    p.deactivate();
                    break;
                }
            }
        }
    }

    // 2. Zombies eating Plants
    for (auto& z : m_zombies) {
        if (z->isDead()) continue;

        bool foundPlantToEat = false;
        int zRow = (int)((z->getY() - 80.0f + 40.0f) / 100.0f);
        if (zRow < 0) zRow = 0;
        if (zRow > 4) zRow = 4;

        for (int c = 0; c < 9; ++c) {
            Plant* p = m_grid[zRow][c].get();
            if (p && !p->isDead()) {
                float plantX = (float)p->getX();
                if (z->getX() >= plantX - 20.0f && z->getX() <= plantX + 50.0f) {
                    foundPlantToEat = true;
                    z->setEating(true);
                    p->takeDamage(z->getDamage());
                    if (p->isDead()) {
                        m_grid[zRow][c] = nullptr;
                        z->setEating(false);
                    }
                    break;
                }
            }
        }

        if (!foundPlantToEat && z->isEating()) {
            z->setEating(false);
        }

        // Check loss condition: Zombie reaches house
        if (z->getX() < 160.0f) {
            m_levelLost = true;
        }
    }
}

void Level1::update(float dt) {
    if (m_levelWon || m_levelLost) return;

    // Update SeedBank
    Vector2 mousePos = GetVirtualMousePosition();
    bool mouseClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool rightClicked = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetVirtualMousePosition();
        int getRow, getCol;
        if (getGridCell(mousePos, getRow, getCol)) {
            m_zombies.push_back(std::make_unique<ZombieNormal>(res, 700.0f, 60.0f + getRow * 100.0f));
        }
    }

    if (rightClicked) {
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

                for (auto &zombie : m_zombies) {
                    if (!zombie->isDead()) {
                        float zombieX = zombie->getX();
                        float zombieY = zombie->getY();
                        bool isSameRow = (std::abs(zombieY - plantY) < 50.0f);

                        if (isSameRow) {
                            float dist = zombieX - plantX;
                            if (dist >= 0.0f && dist <= 700.0f) {
                                shoot = true;
                                break;
                            }
                        }
                    }
                }

                std::string targetAnim = shoot ? "anim_shooting" : "anim_head_idle";
                std::string plantName = m_grid[r][c]->getName();
                if (plantName == "SunFlower" || plantName == "Wallnut" ||
                    plantName == "CherryBomb" || plantName == "Chomper") {
                    targetAnim = "anim_idle";
                }

                // Chú ý: Chỉ đổi animation khi animation hiện tại chưa phải targetAnim!
                // (Tránh gọi SetAnimation mỗi frame làm reset frame về 0 liên tục)
                if (m_grid[r][c]->getAnim().GetCurrentAnimName() != targetAnim) {
                    m_grid[r][c]->SetAnimation(targetAnim);
                }
            }
        }
    }

    m_seedBank.update(dt, mousePos, mouseClicked);

    // Plant placement / Shovel removal logic
    int hoverRow, hoverCol;
    if (mouseClicked && getGridCell(mousePos, hoverRow, hoverCol)) {
        if (m_seedBank.isShovelSelected()) {
            if (m_grid[hoverRow][hoverCol]) {
                m_grid[hoverRow][hoverCol] = nullptr;
                m_seedBank.deselect();
            }
        } else {
            std::string selectedType = m_seedBank.getSelectedPlantType();
            if (!selectedType.empty() && m_grid[hoverRow][hoverCol] == nullptr) {
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
            }
        }
    }

    // Sky sun timer
    m_skySunTimer += dt;
    if (m_skySunTimer >= 10.0f) {
        m_skySunTimer = 0.0f;
        spawnSunFromSky();
    }

    // Wave spawn timer
    if (m_currentWave < m_maxWaves) {
        m_waveTimer -= dt;
        if (m_waveTimer <= 0.0f) {
            spawnNextWave();
            m_waveTimer = 22.0f;
        }
    }

    // Update plants
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                m_grid[r][c]->update(dt, m_projectiles, m_suns);
            }
        }
    }

    // Update zombies
    for (auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->update(dt);
        }
    }

    // Update projectiles
    for (auto& p : m_projectiles) {
        p.update(dt);
    }

    // Update sun items & click collection
    for (auto& s : m_suns) {
        s.update(dt);
        if (mouseClicked && s.isActive() && s.isClicked(mousePos)) {
            m_seedBank.addSun(2500);
            s.collect();
        }
    }

    // Clean up inactive projectiles, suns, and finished zombies
    m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
        [](const Projectile& p) { return !p.isActive(); }), m_projectiles.end());

    m_suns.erase(std::remove_if(m_suns.begin(), m_suns.end(),
        [](const SunItem& s) { return !s.isActive(); }), m_suns.end());

    m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
        [](const std::unique_ptr<Zombie>& z) { return z->isFinished(); }), m_zombies.end());

    // Update collisions
    updateCollisions(dt);

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
            m_levelWon = true;
        }
    }
}

void Level1::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Background cropped at {90, 0, 900, 600}
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
                DrawRectangle(x, y, 80, 80, ((x/80 + y/80) % 2 == 0) ? GREEN : DARKGREEN);
            }
        }
    }

    // 2. Draw hover highlight cell on lawn grid
    Vector2 mousePos = GetVirtualMousePosition();
    int hoverRow, hoverCol;
    if (getGridCell(mousePos, hoverRow, hoverCol)) {
        float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
        float cellY = 80.0f + hoverRow * 100.0f;
        float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
        float cellH = 100.0f;
        DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
    }

    // 3. Draw Plants
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (m_grid[r][c] && !m_grid[r][c]->isDead()) {
                m_grid[r][c]->draw();
            }
        }
    }

    // 4. Draw Zombies
    for (const auto& z : m_zombies) {
        if (!z->isFinished()) {
            z->draw();
        }
    }

    // 5. Draw Projectiles
    for (const auto& p : m_projectiles) {
        p.draw();
    }

    // 6. Draw Sun Items
    for (const auto& s : m_suns) {
        s.draw();
    }

    // 7. Draw Top SeedBank & Plant Seed Packets UI
    m_seedBank.draw(res, mousePos);

    // 8. Draw Win / Loss Overlays
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

    // 9. Draw stretched to screen
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

void Level1::run() {
    SetUIInteractionEnabled(true);
    while (!WindowShouldClose()) {
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        float dt = GetFrameTime();
        update(dt);
        draw();

        if (IsKeyPressed(KEY_ESCAPE) || (m_levelWon && IsKeyPressed(KEY_ENTER))) {
            break;
        }
    }
}
