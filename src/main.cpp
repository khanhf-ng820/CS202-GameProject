#include "raylib.h"
#include "resources.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "UIHelpers.h"
#include "testing.h"
#include "Level1.h"
#include <iostream>
#include <string>

int main() {
    // Enable config flags for high quality drawing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    // Start window at 800x600 default (Compact)
    int windowWidth = 800;
    int windowHeight = 600;
    InitWindow(windowWidth, windowHeight, "Plants vs. Zombies - Level 1 (C++ & Raylib)");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    Resources& res = Resources::GetInstance();
    std::string reanimDir = res.GetAssetPath("assets/reanim");
    res.LoadAll(reanimDir);
    std::string imagesDir = res.GetAssetPath("assets/images");
    res.LoadAll(imagesDir);
    std::string particlesDir = res.GetAssetPath("assets/particles");
    res.LoadAll(particlesDir);
    std::string seedPacketsDir = res.GetAssetPath("assets/PlantSeedPackets");
    res.LoadAll(seedPacketsDir);

    // Create virtual canvas render texture (800x600)
    RenderTexture2D targetScreen = LoadRenderTexture(800, 600);
    SetTextureFilter(targetScreen.texture, TEXTURE_FILTER_BILINEAR);

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
            menu.update(dt);
            if (menu.getAction() == MenuAction::StartAdventure || menu.getAction() == MenuAction::Level1) {
                Level1 level1State(res, targetScreen);
                level1State.run();
                menu.resetAction();
            } else if (menu.getAction() == MenuAction::Options) {
                showOptions = true;
                menu.resetAction();
            } else if (menu.getAction() == MenuAction::Quit) {
                exitGame = true;
            }
        }

        // Update UI interaction availability based on options menu visibility
        SetUIInteractionEnabled(!showOptions);

        // --- Draw to Virtual Canvas ---
        BeginTextureMode(targetScreen);
        ClearBackground(RAYWHITE);

        menu.draw();
        
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
