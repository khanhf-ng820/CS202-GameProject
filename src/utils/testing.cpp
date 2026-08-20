#include "testing.h"
#include "Reanimation.h"
#include "AudioManager.h"
#include "UIHelpers.h"
#include "PeaShooter.h"
#include "SnowPea.h"
#include "Cornpult.h"
#include "FirePea.h"
#include "Jalapeno.h"
#include "SunFlower.h"
#include "GatlingPea.h"
#include "Repeater.h"
#include "Melonpult.h"
#include "Wallnut.h"
#include "CherryBomb.h"
#include "Chomper.h"
#include "Torchwood.h"
#include "Cabbagepult.h"
#include "Caltrop.h"
#include "Garlic.h"
#include "Gravebuster.h"
#include "IceShroom.h"
#include "PotatoMine.h"
#include "SpikeRock.h"
#include "Squash.h"
#include "FlagZombie.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "FootballZombie.h"
#include "NewspaperZombie.h"
#include "PoleVaultingZombie.h"
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

Testing::Testing(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen),
      currentPlantType(3),
      currentZombieType(-1)
{
    currentPlant = std::make_unique<FirePea>(res, 480, 360);
    currentZombie = nullptr;
}

void Testing::run() {
    auto createPlant = [this](int type) -> std::unique_ptr<Plant> {
        switch (type) {
            case 0:  return std::make_unique<PeaShooter>(res, 480, 360);
            case 1:  return std::make_unique<SnowPea>(res, 480, 360);
            case 2:  return std::make_unique<Cornpult>(res, 480, 360);
            case 3:  return std::make_unique<FirePea>(res, 480, 360);
            case 4:  return std::make_unique<Jalapeno>(res, 480, 360);
            case 5:  return std::make_unique<SunFlower>(res, 480, 360);
            case 6:  return std::make_unique<GatlingPea>(res, 480, 360);
            case 7:  return std::make_unique<Repeater>(res, 480, 360);
            case 8:  return std::make_unique<Melonpult>(res, 480, 360);
            case 9:  return std::make_unique<Wallnut>(res, 480, 360);
            case 10: return std::make_unique<CherryBomb>(res, 480, 360);
            case 11: return std::make_unique<Chomper>(res, 480, 360);
            case 12: return std::make_unique<Torchwood>(res, 480, 360);
            case 13: return std::make_unique<Cabbagepult>(res, 480, 360);
            case 14: return std::make_unique<Caltrop>(res, 480, 360);
            case 15: return std::make_unique<Garlic>(res, 480, 360);
            case 16: return std::make_unique<Gravebuster>(res, 480, 360);
            case 17: return std::make_unique<IceShroom>(res, 480, 360);
            case 18: return std::make_unique<PotatoMine>(res, 480, 360);
            case 19: return std::make_unique<SpikeRock>(res, 480, 360);
            case 20: return std::make_unique<Squash>(res, 480, 360);
            default: return std::make_unique<PeaShooter>(res, 480, 360);
        }
    };

    auto createZombie = [this](int type) -> std::unique_ptr<Zombie> {
        switch (type) {
            case 0: return std::make_unique<FlagZombie>(res, 680.0f, 360.0f);
            case 1: return std::make_unique<ZombieNormal>(res, 680.0f, 360.0f);
            case 2: return std::make_unique<ConeheadZombie>(res, 680.0f, 360.0f);
            case 3: return std::make_unique<BucketheadZombie>(res, 680.0f, 360.0f);
            case 4: return std::make_unique<FootballZombie>(res, 680.0f, 360.0f);
            case 5: return std::make_unique<NewspaperZombie>(res, 680.0f, 360.0f);
            case 6: return std::make_unique<PoleVaultingZombie>(res, 680.0f, 360.0f);
            default: return nullptr;
        }
    };

    const std::vector<std::string> plantNames = {
        "Peashooter", "Snow Pea", "Cornpult",
        "Fire Pea", "Jalapeno", "Sunflower",
        "Gatling Pea", "Repeater", "Melon Pult",
        "Wallnut", "Cherry Bomb", "Chomper",
        "Torchwood", "Cabbage-pult", "Caltrop",
        "Garlic", "Gravebuster", "Ice-shroom",
        "Potato Mine", "Spikerock", "Squash"
    };

    const std::vector<std::pair<std::string, int>> zombieButtons = {
        { "Flag Zombie", 0 }, { "Normal Zombie", 1 }, { "Conehead", 2 },
        { "Buckethead", 3 },  { "Football", 4 },      { "Newspaper", 5 },
        { "Pole Vaulting", 6 }, { "Clear Zombie", -1 }
    };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            std::string sfxPath = res.GetAssetPath("assets/sounds/gravebutton.ogg");
            AudioManager::GetInstance().PlaySoundEffect(sfxPath);
            break;
        }

        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        float dt = GetFrameTime();

        if (currentPlant && !currentPlant->isDead()) {
            currentPlant->update(dt, projectiles, suns);
        }
        if (currentZombie && !currentZombie->isFinished()) {
            currentZombie->update(dt);
        }

        for (auto& p : projectiles) {
            p.update(dt);
        }
        for (auto& s : suns) {
            s.update(dt);
        }

        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }), projectiles.end());

        suns.erase(std::remove_if(suns.begin(), suns.end(),
            [](const SunItem& s) { return !s.isActive(); }), suns.end());

        SetUIInteractionEnabled(true);

        BeginTextureMode(targetScreen);
        ClearBackground(RAYWHITE);

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
            for (int y = 0; y < 600; y += 80) {
                for (int x = 0; x < 800; x += 80) {
                    DrawRectangle(x, y, 80, 80, ((x/80 + y/80) % 2 == 0) ? GREEN : DARKGREEN);
                }
            }
        }

        if (currentPlant && !currentPlant->isDead()) {
            currentPlant->draw();
        } else {
            if (DrawButton({ 400, 280, 180, 40 }, "RESPAWN PLANT", RED, MAROON, WHITE)) {
                currentPlant = createPlant(currentPlantType);
                projectiles.clear();
                suns.clear();
            }
        }

        if (currentZombie) {
            if (!currentZombie->isFinished()) {
                currentZombie->draw();
                if (DrawButton({ 600, 220, 180, 40 }, "TAKE DAMAGE (-100)", ORANGE, RED, WHITE)) {
                    currentZombie->takeDamage(100.0f);
                }
            } else {
                if (DrawButton({ 600, 280, 180, 40 }, "RESPAWN ZOMBIE", RED, MAROON, WHITE)) {
                    currentZombie = createZombie(currentZombieType);
                }
            }
        }

        for (const auto& p : projectiles) {
            p.draw();
        }
        for (const auto& s : suns) {
            s.draw();
        }

        // Sidebar panel
        const float panelWidth = 360.0f;
        const float btnW = 105.0f;
        const float btnH = 20.0f;
        const float colX[3] = { 15.0f, 127.0f, 239.0f };

        DrawRectangleRec({ 0, 0, panelWidth, 600.0f }, ColorAlpha(DARKBLUE, 0.85f));
        DrawRectangleLines(0, 0, (int)panelWidth, 600, ColorAlpha(WHITE, 0.3f));

        DrawText("PLANT & ZOMBIE REANIMATOR", 15, 12, 18, SKYBLUE);
        DrawText("C++ & Raylib Visualizer (Testing Menu)", 15, 32, 12, GRAY);
        DrawLine(15, 48, (int)panelWidth - 15, 48, ColorAlpha(WHITE, 0.2f));

        // Plants section
        DrawText("Select Plant Type (21 Plants):", 15, 54, 14, SKYBLUE);

        for (size_t i = 0; i < plantNames.size(); ++i) {
            int c = (int)(i % 3);
            int r = (int)(i / 3);
            float bx = colX[c];
            float by = 72.0f + r * 23.0f;

            bool isCurrent = ((int)i == currentPlantType);
            Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
            Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

            if (DrawButton({ bx, by, btnW, btnH }, plantNames[i].c_str(), baseCol, hoverCol, WHITE)) {
                if (currentPlantType != (int)i) {
                    currentPlantType = (int)i;
                    currentPlant = createPlant(currentPlantType);
                    projectiles.clear();
                    suns.clear();
                }
            }
        }

        // Zombies section
        float plantEndY = 72.0f + ((plantNames.size() + 2) / 3) * 23.0f;
        DrawLine(15, (int)plantEndY + 2, (int)panelWidth - 15, (int)plantEndY + 2, ColorAlpha(WHITE, 0.2f));
        DrawText("Select Zombie Type (7 Zombies):", 15, (int)plantEndY + 8, 14, SKYBLUE);

        float zStartY = plantEndY + 26.0f;
        for (size_t i = 0; i < zombieButtons.size(); ++i) {
            int c = (int)(i % 3);
            int r = (int)(i / 3);
            float bx = colX[c];
            float by = zStartY + r * 23.0f;

            int zType = zombieButtons[i].second;
            bool isCurrent = (currentZombieType == zType);
            Color baseCol = isCurrent ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
            Color hoverCol = isCurrent ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f);

            if (DrawButton({ bx, by, btnW, btnH }, zombieButtons[i].first.c_str(), baseCol, hoverCol, WHITE)) {
                if (currentZombieType != zType) {
                    currentZombieType = zType;
                    currentZombie = createZombie(currentZombieType);
                }
            }
        }

        // Animation section
        float zEndY = zStartY + ((zombieButtons.size() + 2) / 3) * 23.0f;
        DrawLine(15, (int)zEndY + 2, (int)panelWidth - 15, (int)zEndY + 2, ColorAlpha(WHITE, 0.2f));
        DrawText("Select Animation:", 15, (int)zEndY + 8, 14, SKYBLUE);

        Reanimation* activeAnim = nullptr;
        if (currentZombieType != -1 && currentZombie) {
            activeAnim = &currentZombie->getAnim();
        } else if (currentPlant) {
            activeAnim = &currentPlant->getAnim();
        }

        if (activeAnim) {
            const auto& anims = activeAnim->GetAnimations();
            float aStartY = zEndY + 26.0f;
            for (size_t i = 0; i < anims.size(); ++i) {
                int c = (int)(i % 3);
                int r = (int)(i / 3);
                float bx = colX[c];
                float by = aStartY + r * 23.0f;

                if (by + btnH > 600.0f - 8.0f) {
                    DrawText("...", 15, by, 14, GRAY);
                    break;
                }

                std::string label = res.FormatAnimName(anims[i].name);
                bool isCurrent = ((int)i == activeAnim->GetCurrentAnimIndex());
                Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
                Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

                if (DrawButton({ bx, by, btnW, btnH }, label.c_str(), baseCol, hoverCol, WHITE)) {
                    activeAnim->SetAnimationIndex((int)i);
                }
            }
        }

        // Banner instruction
        DrawRectangleRec({ 380, 20, 400, 40 }, ColorAlpha(BLACK, 0.5f));
        DrawText("Click sidebar buttons to switch Plant / Zombie / Animation.", 392, 32, 13, LIGHTGRAY);

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
}

