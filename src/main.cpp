#include "raylib.h"
#include "resources.h"
#include "AudioManager.h"
#include "LoadingScreen.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "ShopMenu.h"
#include "HelpMenu.h"
#include "UIHelpers.h"
#include "testing.h"
#include "Level1.h"
#include "BowlingLevel.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

enum class AppState {
    Loading,
    MainMenu
};

int main() {
    // Enable config flags for high quality drawing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    // Start window at 800x600 default (Compact)
    int windowWidth = 800;
    int windowHeight = 600;
    InitWindow(windowWidth, windowHeight, "Plants vs. Zombies - Level 1 (C++ & Raylib)");
    AudioManager::GetInstance().Init();
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    Resources& res = Resources::GetInstance();

    // 1. Load minimal assets required for the Loading Screen
    res.LoadMinimalForLoadingScreen();

    // 2. Queue all remaining game assets across directories
    std::vector<std::string> assetDirs = {
        "assets/reanim",
        "assets/images",
        "assets/particles",
        "assets/PlantSeedPackets"
    };
    res.PrepareAssetLoadingQueue(assetDirs);

    // Create virtual canvas render texture (800x600)
    RenderTexture2D targetScreen = LoadRenderTexture(800, 600);
    SetTextureFilter(targetScreen.texture, TEXTURE_FILTER_BILINEAR);

    LoadingScreen loadingScreen(res);
    std::unique_ptr<MainMenu> menu;
    std::unique_ptr<OptionsMenu> optionsMenu;
    std::unique_ptr<ShopMenu> shopMenu;
    std::unique_ptr<HelpMenu> helpMenu;

    AppState currentState = AppState::Loading;
    bool showOptions = false;
    bool showShop = false;
    bool showHelp = false;
    bool exitGame = false;

    while (!WindowShouldClose() && !exitGame) {
        // Update virtual mouse scale based on actual window size
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        AudioManager::GetInstance().Update();

        // --- Update ---
        float dt = GetFrameTime();
        
        if (currentState == AppState::Loading) {
            bool doneLoading = loadingScreen.update(dt);
            if (doneLoading) {
                // Immediately transition to Main Menu upon completing asset loading
                menu = std::make_unique<MainMenu>(res);
                optionsMenu = std::make_unique<OptionsMenu>(res);
                shopMenu = std::make_unique<ShopMenu>(res);
                helpMenu = std::make_unique<HelpMenu>(res);
                currentState = AppState::MainMenu;
            }
        } else if (showOptions && optionsMenu) {
            optionsMenu->update(dt, showOptions, windowWidth, windowHeight);
        } else if (showHelp && helpMenu) {
            helpMenu->update(dt, showHelp);
        } else if (showShop && shopMenu) {
            shopMenu->update(dt, showShop);
        } else if (menu) {
            menu->update(dt);
            if (menu->getAction() == MenuAction::StartAdventure) {
                BowlingLevel bowlingState(res, targetScreen);
                bowlingState.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::ZenGarden) {
                Testing testingState(res, targetScreen);
                testingState.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Level1) {
                Level1 level1State(res, targetScreen);
                level1State.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Options) {
                showOptions = true;
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Help) {
                showHelp = true;
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Shop) {
                showShop = true;
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Quit) {
                exitGame = true;
            }
        }

        // Update UI interaction availability based on overlay menu visibility
        SetUIInteractionEnabled(currentState == AppState::MainMenu && !showOptions && !showShop && !showHelp);

        // --- Draw to Virtual Canvas ---
        BeginTextureMode(targetScreen);
        ClearBackground(RAYWHITE);

        if (currentState == AppState::Loading) {
            loadingScreen.draw();
        } else if (showShop && shopMenu) {
            shopMenu->draw();
        } else if (menu) {
            menu->draw();
            if (showOptions && optionsMenu) {
                optionsMenu->draw();
            }
            if (showHelp && helpMenu) {
                helpMenu->draw();
            }
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

    AudioManager::GetInstance().Close();
    CloseWindow();
    return 0;
}
