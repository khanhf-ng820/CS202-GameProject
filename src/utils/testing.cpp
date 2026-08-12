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
#include "FlagZombie.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "FootballZombie.h"
#include "NewspaperZombie.h"
#include <algorithm>
#include <string>
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

        if (!currentPlant->isDead()) {
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

        if (!currentPlant->isDead()) {
            currentPlant->draw();
        } else {
            if (DrawButton({ 430, 280, 180, 40 }, "RESPAWN PLANT", RED, MAROON, WHITE)) {
                if (currentPlantType == 0) currentPlant = std::make_unique<PeaShooter>(res, 480, 360);
                else if (currentPlantType == 1) currentPlant = std::make_unique<SnowPea>(res, 480, 360);
                else if (currentPlantType == 2) currentPlant = std::make_unique<Cornpult>(res, 480, 360);
                else if (currentPlantType == 3) currentPlant = std::make_unique<FirePea>(res, 480, 360);
                else if (currentPlantType == 4) currentPlant = std::make_unique<Jalapeno>(res, 480, 360);
                else if (currentPlantType == 5) currentPlant = std::make_unique<SunFlower>(res, 480, 360);
                else if (currentPlantType == 6) currentPlant = std::make_unique<GatlingPea>(res, 480, 360);
                else if (currentPlantType == 7) currentPlant = std::make_unique<Repeater>(res, 480, 360);
                else if (currentPlantType == 8) currentPlant = std::make_unique<Melonpult>(res, 480, 360);
                else if (currentPlantType == 9) currentPlant = std::make_unique<Wallnut>(res, 480, 360);
                else if (currentPlantType == 10) currentPlant = std::make_unique<CherryBomb>(res, 480, 360);
                else if (currentPlantType == 11) currentPlant = std::make_unique<Chomper>(res, 480, 360);
                else if (currentPlantType == 12) currentPlant = std::make_unique<Torchwood>(res, 480, 360);
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
                    if (currentZombieType == 0) currentZombie = std::make_unique<FlagZombie>(res, 680, 360);
                    else if (currentZombieType == 1) currentZombie = std::make_unique<ZombieNormal>(res, 680, 360);
                    else if (currentZombieType == 2) currentZombie = std::make_unique<ConeheadZombie>(res, 680, 360);
                    else if (currentZombieType == 3) currentZombie = std::make_unique<BucketheadZombie>(res, 680, 360);
                    else if (currentZombieType == 4) currentZombie = std::make_unique<FootballZombie>(res, 680, 360);
                    else if (currentZombieType == 5) currentZombie = std::make_unique<NewspaperZombie>(res, 680, 360);
                }
            }
        }

        for (const auto& p : projectiles) {
            p.draw();
        }
        for (const auto& s : suns) {
            s.draw();
        }

        DrawRectangleRec({ 0, 0, 320, 600.0f }, ColorAlpha(DARKBLUE, 0.75f));
        DrawRectangleLines(0, 0, 320, 600, ColorAlpha(WHITE, 0.3f));

        DrawText("PLANT REANIMATOR", 20, 15, 20, SKYBLUE);
        DrawText("C++ & Raylib Visualizer (OOP)", 20, 38, 13, GRAY);
        DrawLine(20, 58, 300, 58, ColorAlpha(WHITE, 0.2f));

        DrawText("Select Plant Type:", 20, 68, 15, SKYBLUE);

        if (DrawButton({ 20, 90, 135, 22 }, "Peashooter", currentPlantType == 0 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 0 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 0) { currentPlantType = 0; currentPlant = std::make_unique<PeaShooter>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 90, 135, 22 }, "Snow Pea", currentPlantType == 1 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 1 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 1) { currentPlantType = 1; currentPlant = std::make_unique<SnowPea>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 115, 135, 22 }, "Cornpult", currentPlantType == 2 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 2 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 2) { currentPlantType = 2; currentPlant = std::make_unique<Cornpult>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 115, 135, 22 }, "Fire Pea", currentPlantType == 3 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 3 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 3) { currentPlantType = 3; currentPlant = std::make_unique<FirePea>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 140, 135, 22 }, "Jalapeno", currentPlantType == 4 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 4 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 4) { currentPlantType = 4; currentPlant = std::make_unique<Jalapeno>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 140, 135, 22 }, "Sunflower", currentPlantType == 5 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 5 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 5) { currentPlantType = 5; currentPlant = std::make_unique<SunFlower>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 165, 135, 22 }, "Gatling Pea", currentPlantType == 6 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 6 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 6) { currentPlantType = 6; currentPlant = std::make_unique<GatlingPea>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 165, 135, 22 }, "Repeater", currentPlantType == 7 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 7 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 7) { currentPlantType = 7; currentPlant = std::make_unique<Repeater>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 190, 135, 22 }, "Melon Pult", currentPlantType == 8 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 8 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 8) { currentPlantType = 8; currentPlant = std::make_unique<Melonpult>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 190, 135, 22 }, "Wallnut", currentPlantType == 9 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 9 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 9) { currentPlantType = 9; currentPlant = std::make_unique<Wallnut>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 215, 135, 22 }, "Cherry Bomb", currentPlantType == 10 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 10 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 10) { currentPlantType = 10; currentPlant = std::make_unique<CherryBomb>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 165, 215, 135, 22 }, "Chomper", currentPlantType == 11 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 11 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 11) { currentPlantType = 11; currentPlant = std::make_unique<Chomper>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }
        if (DrawButton({ 20, 240, 135, 22 }, "Torchwood", currentPlantType == 12 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 12 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 12) { currentPlantType = 12; currentPlant = std::make_unique<Torchwood>(res, 480, 360); projectiles.clear(); suns.clear(); }
        }

        DrawLine(20, 270, 300, 270, ColorAlpha(WHITE, 0.2f));

        DrawText("Select Zombie Type:", 20, 276, 15, SKYBLUE);
        if (DrawButton({ 20, 296, 135, 22 }, "Flag Zombie", currentZombieType == 0 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 0 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 0) { currentZombieType = 0; currentZombie = std::make_unique<FlagZombie>(res, 680, 360); }
        }
        if (DrawButton({ 165, 296, 135, 22 }, "Normal Zombie", currentZombieType == 1 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 1 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 1) { currentZombieType = 1; currentZombie = std::make_unique<ZombieNormal>(res, 680, 360); }
        }
        if (DrawButton({ 20, 321, 135, 22 }, "Conehead", currentZombieType == 2 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 2 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 2) { currentZombieType = 2; currentZombie = std::make_unique<ConeheadZombie>(res, 680, 360); }
        }
        if (DrawButton({ 165, 321, 135, 22 }, "Buckethead", currentZombieType == 3 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 3 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 3) { currentZombieType = 3; currentZombie = std::make_unique<BucketheadZombie>(res, 680, 360); }
        }
        if (DrawButton({ 20, 346, 135, 22 }, "Football Zombie", currentZombieType == 4 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 4 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 4) { currentZombieType = 4; currentZombie = std::make_unique<FootballZombie>(res, 680, 360); }
        }
        if (DrawButton({ 165, 346, 135, 22 }, "Newspaper Zombie", currentZombieType == 5 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 5 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentZombieType != 5) { currentZombieType = 5; currentZombie = std::make_unique<NewspaperZombie>(res, 680, 360); }
        }
        if (DrawButton({ 20, 371, 135, 22 }, "Clear Zombie", currentZombieType == -1 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == -1 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            currentZombieType = -1; currentZombie.reset();
        }

        DrawLine(20, 398, 300, 398, ColorAlpha(WHITE, 0.2f));
        DrawText("Select Animation:", 20, 403, 15, SKYBLUE);

        Reanimation& activeAnim = (currentZombieType != -1 && currentZombie) ? currentZombie->getAnim() : currentPlant->getAnim();
        const auto& anims = activeAnim.GetAnimations();
        for (size_t i = 0; i < anims.size(); ++i) {
            std::string label = res.FormatAnimName(anims[i].name);
            bool isCurrent = ((int)i == activeAnim.GetCurrentAnimIndex());
            Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
            Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

            int r = (int)(i / 2);
            int c = (int)(i % 2);
            float btnX = (c == 0) ? 20.0f : 165.0f;
            float btnY = 423.0f + r * 26.0f;

            if (btnY + 22.0f > 600.0f - 10.0f) {
                DrawText("...", 20, btnY, 14, GRAY);
                break;
            }
            if (DrawButton({ btnX, btnY, 135, 22 }, label.c_str(), baseCol, hoverCol, WHITE)) {
                activeAnim.SetAnimationIndex((int)i);
            }
        }

        DrawRectangleRec({ 340, 20, 450, 40 }, ColorAlpha(BLACK, 0.5f));
        DrawText("Click buttons on the left to switch animation/plant type.", 355, 32, 14, LIGHTGRAY);

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
