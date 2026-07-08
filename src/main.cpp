#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
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
#include "Projectile.h"
#include "FlagZombie.h"
#include "ZombieNormal.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "MainMenu.h"
#include "OptionsMenu.h"

enum class GameState {
    MainMenu,
    PlantTest
};

int main() {
    // Enable config flags for high quality drawing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    // Start window at 800x600 as requested
    int windowWidth = 800;
    int windowHeight = 600;
    InitWindow(windowWidth, windowHeight, "Plants vs. Zombies Reanim Visualizer - C++ & Raylib");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    Resources& res = Resources::GetInstance();
    std::string reanimDir = res.GetAssetPath("assets/reanim");
    res.LoadAll(reanimDir);
    std::string imagesDir = res.GetAssetPath("assets/images");
    res.LoadAll(imagesDir);
    std::string particlesDir = res.GetAssetPath("assets/particles");
    res.LoadAll(particlesDir);

    // Create virtual canvas render texture (800x600)
    RenderTexture2D targetScreen = LoadRenderTexture(800, 600);

    int currentPlantType = 3; // Default to Fire Pea for testing
    std::unique_ptr<Plant> currentPlant = std::make_unique<FirePea>(res, 480, 360);
    std::vector<Projectile> projectiles;
    std::vector<SunItem> suns;
    std::unique_ptr<Zombie> currentZombie = nullptr;
    int currentZombieType = -1;

    // --- Game State ---
    GameState currentState = GameState::MainMenu;
    MainMenu menu(res);
    OptionsMenu optionsMenu(res);
    bool showOptions = false;
    bool exitGame = false;

    while (!WindowShouldClose() && !exitGame) {
        // Update virtual mouse scale based on actual window size
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        // --- Update ---
        float dt = GetFrameTime();
        
        if (showOptions) {
            optionsMenu.update(dt, showOptions, windowWidth, windowHeight);
        } else {
            switch (currentState) {
                case GameState::MainMenu: {
                    menu.update(dt);
                    if (menu.getAction() == MenuAction::StartAdventure) {
                        currentState = GameState::PlantTest;
                    } else if (menu.getAction() == MenuAction::Options) {
                        showOptions = true;
                        menu.resetAction();
                    } else if (menu.getAction() == MenuAction::Quit) {
                        exitGame = true;
                    }
                    break;
                }
                case GameState::PlantTest: {
                    if (!currentPlant->isDead()) {
                        currentPlant->update(dt, projectiles, suns);
                    }
                    if (currentZombie && !currentZombie->isDead()) {
                        currentZombie->update(dt);
                    }

                    for (auto& p : projectiles) {
                        p.update(dt);
                    }
                    for (auto& s : suns) {
                        s.update(dt);
                    }
                    
                    // Dọn dẹp các viên đạn đã bay ra ngoài màn hình
                    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
                        [](const Projectile& p) { return !p.isActive(); }), projectiles.end());

                    // Dọn dẹp các mặt trời đã hết thời gian
                    suns.erase(std::remove_if(suns.begin(), suns.end(),
                        [](const SunItem& s) { return !s.isActive(); }), suns.end());
                    break;
                }
            }
        }

        // Update UI interaction availability based on options menu visibility
        SetUIInteractionEnabled(!showOptions);

        // --- Draw to Virtual Canvas ---
        BeginTextureMode(targetScreen);
        ClearBackground(RAYWHITE);

        switch (currentState) {
            case GameState::MainMenu: {
                menu.draw();
                break;
            }
            case GameState::PlantTest: {
                // 1. Draw Lawn Background (Scale to fit screen)
                Texture2D bgTex = res.GetBackground();
                if (bgTex.id != 0) {
                    DrawTexturePro(
                        bgTex,
                        { 0.0f, 0.0f, (float)bgTex.width, (float)bgTex.height },
                        { 0.0f, 0.0f, 800.0f, 600.0f },
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                } else {
                    // Fallback: draw green lawn grid
                    for (int y = 0; y < 600; y += 80) {
                        for (int x = 0; x < 800; x += 80) {
                            DrawRectangle(x, y, 80, 80, ((x/80 + y/80) % 2 == 0) ? GREEN : DARKGREEN);
                        }
                    }
                }

                if (!currentPlant->isDead()) {
                    currentPlant->draw();
                } else {
                    if (DrawButton({ 430, 280, 200, 50 }, "RESPAWN PLANT", RED, MAROON, WHITE)) {
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
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (currentZombie && !currentZombie->isDead()) {
                    currentZombie->draw();
                }

                for (const auto& p : projectiles) {
                    p.draw();
                }

                for (const auto& s : suns) {
                    s.draw();
                }

                // 3. Draw Controls UI Panel (Glassmorphism overlay on the left)
                DrawRectangleRec({ 0, 0, 320, 600.0f }, ColorAlpha(DARKBLUE, 0.75f));
                DrawRectangleLines(0, 0, 320, 600, ColorAlpha(WHITE, 0.3f));

                // Title
                DrawText("PLANT REANIMATOR", 20, 20, 22, SKYBLUE);
                DrawText("C++ & Raylib Visualizer (OOP)", 20, 48, 14, GRAY);
                DrawLine(20, 72, 300, 72, ColorAlpha(WHITE, 0.2f));

                // Plant selector section
                DrawText("Select Plant Type:", 20, 85, 16, SKYBLUE);
                
                if (DrawButton({ 20, 115, 135, 30 }, "Peashooter", currentPlantType == 0 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 0 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 0) {
                        currentPlantType = 0;
                        currentPlant = std::make_unique<PeaShooter>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 115, 135, 30 }, "Snow Pea", currentPlantType == 1 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 1 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 1) {
                        currentPlantType = 1;
                        currentPlant = std::make_unique<SnowPea>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 20, 150, 135, 30 }, "Cornpult", currentPlantType == 2 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 2 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 2) {
                        currentPlantType = 2;
                        currentPlant = std::make_unique<Cornpult>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 150, 135, 30 }, "Fire Pea", currentPlantType == 3 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 3 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 3) {
                        currentPlantType = 3;
                        currentPlant = std::make_unique<FirePea>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 20, 185, 135, 30 }, "Jalapeno", currentPlantType == 4 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 4 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 4) {
                        currentPlantType = 4;
                        currentPlant = std::make_unique<Jalapeno>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 185, 135, 30 }, "Sunflower", currentPlantType == 5 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 5 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 5) {
                        currentPlantType = 5;
                        currentPlant = std::make_unique<SunFlower>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 20, 220, 135, 30 }, "Gatling Pea", currentPlantType == 6 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 6 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 6) {
                        currentPlantType = 6;
                        currentPlant = std::make_unique<GatlingPea>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 220, 135, 30 }, "Repeater", currentPlantType == 7 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 7 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 7) {
                        currentPlantType = 7;
                        currentPlant = std::make_unique<Repeater>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 20, 255, 135, 30 }, "Melon Pult", currentPlantType == 8 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 8 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 8) {
                        currentPlantType = 8;
                        currentPlant = std::make_unique<Melonpult>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 255, 135, 30 }, "Wallnut", currentPlantType == 9 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 9 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 9) {
                        currentPlantType = 9;
                        currentPlant = std::make_unique<Wallnut>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 20, 290, 135, 30 }, "Cherry Bomb", currentPlantType == 10 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 10 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 10) {
                        currentPlantType = 10;
                        currentPlant = std::make_unique<CherryBomb>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }
                if (DrawButton({ 165, 290, 135, 30 }, "Chomper", currentPlantType == 11 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 11 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentPlantType != 11) {
                        currentPlantType = 11;
                        currentPlant = std::make_unique<Chomper>(res, 480, 360);
                        projectiles.clear();
                        suns.clear();
                    }
                }

                DrawLine(20, 330, 300, 330, ColorAlpha(WHITE, 0.2f));
                
                DrawText("Select Zombie Type:", 20, 340, 16, SKYBLUE);
                if (DrawButton({ 20, 365, 135, 30 }, "Flag Zombie", currentZombieType == 0 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 0 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentZombieType != 0) {
                        currentZombieType = 0;
                        currentZombie = std::make_unique<FlagZombie>(res, 680, 360);
                    }
                }
                if (DrawButton({ 165, 365, 135, 30 }, "Normal Zombie", currentZombieType == 1 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == 1 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    if (currentZombieType != 1) {
                        currentZombieType = 1;
                        currentZombie = std::make_unique<ZombieNormal>(res, 680, 360);
                    }
                }
                if (DrawButton({ 20, 400, 135, 30 }, "Clear Zombie", currentZombieType == -1 ? ColorAlpha(RED, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentZombieType == -1 ? ColorAlpha(RED, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
                    currentZombieType = -1;
                    currentZombie.reset();
                }

                DrawLine(20, 440, 300, 440, ColorAlpha(WHITE, 0.2f));

                // Animations list header
                DrawText("Select Animation:", 20, 450, 16, SKYBLUE);

                // Draw a list of animation buttons
                int startY = 470;
                Reanimation& activeAnim = (currentZombieType != -1 && currentZombie) ? currentZombie->getAnim() : currentPlant->getAnim();
                const auto& anims = activeAnim.GetAnimations();
                for (size_t i = 0; i < anims.size(); ++i) {
                    std::string label = res.FormatAnimName(anims[i].name);
                    bool isCurrent = ((int)i == activeAnim.GetCurrentAnimIndex());
                    Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
                    Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

                    // Break if we exceed screen bounds (fixed 600px height)
                    if (startY + 35 > 600.0f - 50) {
                        DrawText("...", 20, startY, 14, GRAY);
                        break;
                    }

                    if (DrawButton({ 20, (float)startY, 280, 30 }, label.c_str(), baseCol, hoverCol, WHITE)) {
                        activeAnim.SetAnimationIndex((int)i);
                    }
                    startY += 35;
                }

                // Footer instructions
                DrawRectangleRec({ 340, 20, 450, 40 }, ColorAlpha(BLACK, 0.5f));
                DrawText("Click buttons on the left to switch animation/plant type.", 355, 32, 14, LIGHTGRAY);
                
                // Back to Menu button
                if (DrawButton({ 20, 600.0f - 45.0f, 120, 35 }, "< MENU",
                               ColorAlpha(DARKBLUE, 0.8f), ColorAlpha(BLUE, 0.8f), WHITE)) {
                    currentState = GameState::MainMenu;
                    menu.resetAction();
                }

                // Options button in PlantTest
                if (DrawButton({ 150, 600.0f - 45.0f, 120, 35 }, "OPTIONS",
                               ColorAlpha(DARKBLUE, 0.8f), ColorAlpha(BLUE, 0.8f), WHITE)) {
                    showOptions = true;
                }

                break;
            } // End of GameState::PlantTest case
        } // End of draw switch

        // Draw Options overlay if active
        if (showOptions) {
            optionsMenu.draw();
        }

        EndTextureMode();

        // --- Draw Virtual Canvas Stretched to Actual Screen ---
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

    // Clean up virtual canvas
    UnloadRenderTexture(targetScreen);

    // Unload all resources
    res.UnloadAll();

    CloseWindow();
    return 0;
}
