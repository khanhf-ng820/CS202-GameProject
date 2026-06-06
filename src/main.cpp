#include "raylib.h"

// Define the raygui implementation token ONLY here
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Include standard file streams and nlohmann/json
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

// Type alias for convenience
using json = nlohmann::json;

// Simple save profile structure
struct GameProfile {
    int currentScore = 0;
    bool isFeatureActive = false;
};

int main() {
    // 1. Window Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "C++20 Engine: raylib + raygui + json");
    SetTargetFPS(60);

    // Local runtime variables managed by structural state
    GameProfile profile;

    // 2. Main Frame Loop
    while (!WindowShouldClose()) {
        // --- Render Presentation Space ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("C++20 Engine Infrastructure Initialized!", 150, 100, 20, DARKGRAY);
        DrawText(TextFormat("Current State Score: %d", profile.currentScore), 150, 140, 20, BLUE);

        // Immediate Mode Button 1: Increment values to create save state changes
        if (GuiButton(Rectangle{ 150, 210, 200, 40 }, "Action: Accumulate Points")) {
            profile.currentScore += 15;
        }

        // Immediate Mode Button 2: Serialize structural state to a local text file
        if (GuiButton(Rectangle{ 380, 210, 120, 40 }, "Save Progress")) {
            json saveFile;
            saveFile["score"] = profile.currentScore;
            saveFile["feature_state"] = profile.isFeatureActive;

            std::ofstream outStream("save_slot_0.json");
            if (outStream.is_open()) {
                outStream << saveFile.dump(4); // Pretty print with 4-space indent
                outStream.close();
                std::cout << "Data serialization operation completed successfully!\n";
            }
        }

        // Immediate Mode Button 3: De-serialize and load progress
        if (GuiButton(Rectangle{ 510, 210, 120, 40 }, "Load Progress")) {
            std::ifstream inStream("save_slot_0.json");
            if (inStream.is_open()) {
                json loadFile;
                inStream >> loadFile;
                inStream.close();

                profile.currentScore = loadFile.value("score", 0);
                profile.isFeatureActive = loadFile.value("feature_state", false);
                std::cout << "Data retrieval operation completed successfully!\n";
            }
        }

        // Immediate Mode Toggle checkbox element mapped directly to object reference
        GuiCheckBox(Rectangle{ 150, 270, 20, 20 }, "Toggle Status Flag Modifier", &profile.isFeatureActive);

        if (profile.isFeatureActive) {
            DrawRectangle(150, 310, 480, 10, GREEN);
        } else {
            DrawRectangle(150, 310, 480, 10, MAROON);
        }

        EndDrawing();
    }

    // 3. Resource Destruction
    CloseWindow();
    return 0;
}
