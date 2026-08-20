#include "raylib.h"
#include "resources.h"
#include "AudioManager.h"
#include "LoadingScreen.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
#include "ShopMenu.h"
#include "HelpMenu.h"
#include "AlmanacMenu.h"
#include "QuizMenu.h"
#include "LevelSelectMenu.h"
#include "UIHelpers.h"
#include "testing.h"
#include "Level1.h"
#include "BowlingLevel.h"
#include "VasebreakerLevel.h"
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
    SetExitKey(KEY_NULL); // Disable Raylib default KEY_ESCAPE exit so ESC key navigates menus instead of closing app
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
    std::unique_ptr<AlmanacMenu> almanacMenu;
    std::unique_ptr<QuizMenu> quizMenu;
    std::unique_ptr<LevelSelectMenu> levelSelectMenu;

    AppState currentState = AppState::Loading;
    bool showOptions = false;
    bool showShop = false;
    bool showHelp = false;
    bool showAlmanac = false;
    bool showQuiz = false;
    bool showLevelSelect = false;
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
                almanacMenu = std::make_unique<AlmanacMenu>(res);
                quizMenu = std::make_unique<QuizMenu>(res);
                levelSelectMenu = std::make_unique<LevelSelectMenu>(res);
                currentState = AppState::MainMenu;
            }
        } else if (showLevelSelect && levelSelectMenu) {
            levelSelectMenu->update(dt, showLevelSelect);
            if (levelSelectMenu->getAction() == LevelSelectAction::PlayLevel1) {
                Level1 level1State(res, targetScreen);
                level1State.run();
                AudioManager::GetInstance().PlayMusic(MusicTrack::MainMenu);
                levelSelectMenu->resetAction();
            }
        } else if (showOptions && optionsMenu) {
            optionsMenu->update(dt, showOptions, windowWidth, windowHeight);
        } else if (showHelp && helpMenu) {
            helpMenu->update(dt, showHelp);
        } else if (showShop && shopMenu) {
            shopMenu->update(dt, showShop);
        } else if (showAlmanac && almanacMenu) {
            almanacMenu->update(dt, showAlmanac);
        } else if (showQuiz && quizMenu) {
            quizMenu->update(dt, showQuiz);
        } else if (menu) {
            menu->update(dt);
            if (menu->getAction() == MenuAction::StartAdventure) {
                showLevelSelect = true;
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::ZenGarden) {
                Testing testingState(res, targetScreen);
                testingState.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Vasebreaker) {
                VasebreakerLevel vasebreakerState(res, targetScreen);
                vasebreakerState.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Level1) {
                BowlingLevel bowlingState(res, targetScreen);
                bowlingState.run();
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Level2) {
                showQuiz = true;
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
            } else if (menu->getAction() == MenuAction::Almanac) {
                showAlmanac = true;
                menu->resetAction();
            } else if (menu->getAction() == MenuAction::Quit || IsKeyPressed(KEY_ESCAPE)) {
                exitGame = true;
            }
        }

        // Update UI interaction availability based on overlay menu visibility
        SetUIInteractionEnabled(currentState == AppState::MainMenu && !showOptions && !showShop && !showHelp && !showAlmanac && !showQuiz && !showLevelSelect);

        // --- Draw to Virtual Canvas ---
        BeginTextureMode(targetScreen);
        ClearBackground(RAYWHITE);

        if (currentState == AppState::Loading) {
            loadingScreen.draw();
        } else if (showLevelSelect && levelSelectMenu) {
            levelSelectMenu->draw();
        } else if (showShop && shopMenu) {
            shopMenu->draw();
        } else if (showAlmanac && almanacMenu) {
            almanacMenu->draw();
        } else if (showQuiz && quizMenu) {
            quizMenu->draw();
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
