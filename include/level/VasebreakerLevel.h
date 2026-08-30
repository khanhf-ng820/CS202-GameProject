#pragma once
#include "raylib.h"
#include "resources.h"
#include "Zombie.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "Reanimation.h"
#include "Vase.h"
#include "Plant.h"
#include "PeaShooter.h"
#include "Repeater.h"
#include "SnowPea.h"
#include "Wallnut.h"
#include "Projectile.h"
#include "SunItem.h"
#include "InGameMenu.h"
#include "BitmapFont.h"
#include <vector>
#include <memory>
#include <string>
#include <cctype>

struct VaseShard {
    float x;
    float y;
    float vx;
    float vy;
    float rotation;
    float rotSpeed;
    float groundY;
    float lifetime;
    float maxLifetime;
    int frameCol;
    int frameRow;
};

struct DroppedSeedPacket {
    float x;
    float y;
    float startY;
    float groundY;
    float vy;
    float width;
    float height;
    std::string plantType; // e.g. "PeaShooter", "SnowPea", "Wallnut", "Repeater"
    mutable Texture2D cachedTex = {0};

    bool isClicked(Vector2 mousePos) const {
        return CheckCollisionPointRec(mousePos, { x, y, width, height });
    }

    void update(float dt) {
        if (y < groundY) {
            vy += 400.0f * dt;
            y += vy * dt;
            if (y >= groundY) {
                y = groundY;
                vy = 0.0f;
            }
        }
    }

    void draw(Resources& res, bool isSelected) const {
        if (cachedTex.id == 0) {
            std::string upperType = plantType;
            for (auto& c : upperType) c = toupper((unsigned char)c);
            cachedTex = res.GetTexture(upperType);
            if (cachedTex.id == 0) cachedTex = res.GetTexture(plantType);
            if (cachedTex.id == 0) {
                std::string path = res.GetAssetPath("assets/PlantSeedPackets/" + plantType + ".png");
                res.LoadFile(path);
                cachedTex = res.GetTexture(upperType);
                if (cachedTex.id == 0) cachedTex = res.GetTexture(plantType);
            }
        }

        // Draw shadow / glow backing
        if (isSelected) {
            DrawRectangleRec({ x - 3.0f, y - 3.0f, width + 6.0f, height + 6.0f }, ColorAlpha(YELLOW, 0.8f));
        } else {
            DrawRectangleRec({ x - 2.0f, y - 2.0f, width + 4.0f, height + 4.0f }, ColorAlpha(GREEN, 0.4f));
        }

        if (cachedTex.id != 0) {
            Rectangle srcRec = { 0.0f, 0.0f, (float)cachedTex.width, (float)cachedTex.height };
            Rectangle destRec = { x, y, width, height };
            DrawTexturePro(cachedTex, srcRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangleRec({ x, y, width, height }, DARKGREEN);
            DrawText(plantType.c_str(), (int)x + 2, (int)y + 20, 10, WHITE);
        }

        if (isSelected) {
            DrawRectangleLinesEx({ x - 3.0f, y - 3.0f, width + 6.0f, height + 6.0f }, 2.0f, GOLD);
        }
    }
};

class VasebreakerLevel {
public:
    VasebreakerLevel(Resources& res, RenderTexture2D targetScreen);
    ~VasebreakerLevel() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;
    Texture2D m_bgTex = {0};
    Texture2D m_chunksTex = {0};
    BitmapFont m_font;

    // In-game Pause Menu
    std::unique_ptr<InGameMenu> m_inGameMenu;
    bool m_exitToMainMenu = false;

    // Mallet cursor animation state
    Reanimation m_malletAnim;
    bool m_isSwinging = false;

    // Plant placement cursor preview
    Reanimation m_previewPlantAnim;

    // 5x9 Vase Grid
    std::unique_ptr<Vase> m_vases[5][9];

    // 5x9 Placed Plants Grid
    std::unique_ptr<Plant> m_plants[5][9];

    // Dropped seed packet cards on lawn (z-index in front of vases)
    std::vector<DroppedSeedPacket> m_droppedPackets;
    int m_selectedPacketIndex = -1;

    // Active projectiles & sun items (for plant update interface)
    std::vector<Projectile> m_projectiles;
    std::vector<SunItem> m_suns;

    // Shard particles from destroyed vases
    std::vector<VaseShard> m_shards;

    // Pending strike target to synchronize mallet impact and prevent double-click destruction
    int m_pendingVaseRow = -1;
    int m_pendingVaseCol = -1;
    float m_pendingVaseTimer = 0.0f;

    // Active zombies on the lawn
    std::vector<std::unique_ptr<Zombie>> m_zombies;

    // Game state
    bool m_levelWon = false;
    bool m_levelLost = false;
    bool m_winMusicPlayed = false;

    // Helper functions
    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
    void spawnVases();
    void breakVase(int row, int col);
    void createPlant(const std::string& type, int row, int col, int pixelX, int pixelY);
    void restartLevel();
};
