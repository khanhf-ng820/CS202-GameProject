#include "raylib.h"

// Define the raygui implementation token ONLY here
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
    // 1. Core Window Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "C++20 OOP Engine: raylib + raygui Setup");
    SetTargetFPS(60);

    // 2. Local State Control Variables
    bool interactState = false;

    // 3. Main Frame Loop
    while (!WindowShouldClose()) {
        // --- Update Logic Space ---
        // (State machines and entity updates live here)

        // --- Render Presentation Space ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("C++20 Game Infrastructure Initialized Successfully!", 150, 140, 20, DARKGRAY);

        // Immediate Mode GUI Element Rendering
        // Returns true instantly when clicked
        if (GuiButton(Rectangle{ 325, 210, 150, 40 }, interactState ? "Status: ACTIVE" : "Click To Activate")) {
            interactState = !interactState;
        }

        // Draw basic status indicators based on tool state
        if (interactState) {
            DrawRectangle(325, 270, 150, 10, GREEN);
        } else {
            DrawRectangle(325, 270, 150, 10, MAROON);
        }

        EndDrawing();
    }

    // 4. Resource Allocation Deconstruction
    CloseWindow();
    return 0;
}
