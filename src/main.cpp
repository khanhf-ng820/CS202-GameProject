#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "PeaShooter.h"
#include "SnowPea.h"
#include "Projectile.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#pragma once 

// Draw a simple button and return true if clicked
bool DrawButton(Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);
    Color col = hovered ? hoverColor : baseColor;
    
    DrawRectangleRec(rect, col);
    DrawRectangleLinesEx(rect, 2.0f, ColorAlpha(textColor, 0.5f));
    
    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - fontSize)/2, fontSize, textColor);
    
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int main() {
    // Enable config flags for high quality drawing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280, 768, "Plants vs. Zombies Reanim Visualizer - C++ & Raylib");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    Resources& res = Resources::GetInstance();
    std::string reanimDir = res.GetAssetPath("assets/reanim");
    res.LoadAll(reanimDir);
    std::string imagesDir = res.GetAssetPath("assets/images");
    res.LoadAll(imagesDir);

    SnowPea mySnowPea(res, 550, 420);
    std::vector<Projectile> projectiles;


    while (!WindowShouldClose()) {
        // --- Update ---
        float dt = GetFrameTime();
        
        // Relocate zombie on click (if not clicking the UI panel on the left
        mySnowPea.update(dt, projectiles);

        for (auto& p : projectiles) {
            p.update(dt);
        }
        
        // Cọn dẹp các viên đạn đã bay ra ngoài màn hình
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.isActive(); }), projectiles.end());

        // --- Draw ---
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // 1. Draw Lawn Background (Scale to fit screen)
        Texture2D bgTex = res.GetBackground();
        if (bgTex.id != 0) {
            DrawTexturePro(
                bgTex,
                { 0.0f, 0.0f, (float)bgTex.width, (float)bgTex.height },
                { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            // Fallback: draw green lawn grid
            for (int y = 0; y < GetScreenHeight(); y += 80) {
                for (int x = 0; x < GetScreenWidth(); x += 80) {
                    DrawRectangle(x, y, 80, 80, ((x/80 + y/80) % 2 == 0) ? GREEN : DARKGREEN);
                }
            }
        }

        mySnowPea.draw();
        
        for (const auto& p : projectiles) {
            p.draw();
        }

        // 3. Draw Controls UI Panel (Glassmorphism overlay on the left)
        DrawRectangleRec({ 0, 0, 320, (float)GetScreenHeight() }, ColorAlpha(DARKBLUE, 0.75f));
        DrawRectangleLines(0, 0, 320, GetScreenHeight(), ColorAlpha(WHITE, 0.3f));

        // Title
        DrawText("PLANT REANIMATOR", 20, 20, 22, SKYBLUE);
        DrawText("C++ & Raylib Visualizer (OOP)", 20, 48, 14, GRAY);
        DrawLine(20, 72, 300, 72, ColorAlpha(WHITE, 0.2f));
        DrawLine(20, 195, 300, 195, ColorAlpha(WHITE, 0.2f));
        DrawLine(20, 400, 300, 400, ColorAlpha(WHITE, 0.2f));

        // Animations list header
        DrawText("Select Animation:", 20, 415, 16, SKYBLUE);

        // Draw a list of animation buttons
        int startY = 445;
        const auto& anims = mySnowPea.getAnim().GetAnimations();
        for (size_t i = 0; i < anims.size(); ++i) {
            std::string label = res.FormatAnimName(anims[i].name);
            bool isCurrent = ((int)i == mySnowPea.getAnim().GetCurrentAnimIndex());
            Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
            Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

            // Break if we exceed screen bounds
            if (startY + 35 > GetScreenHeight() - 50) {
                DrawText("...", 20, startY, 14, GRAY);
                break;
            }

            if (DrawButton({ 20, (float)startY, 280, 30 }, label.c_str(), baseCol, hoverCol, WHITE)) {
                mySnowPea.getAnim().SetAnimationIndex((int)i);
            }
            startY += 35;
        }

        // Footer instructions
        DrawRectangleRec({ 340, 20, 450, 40 }, ColorAlpha(BLACK, 0.5f));
        DrawText("Click buttons on the left to switch animation.", 355, 32, 14, LIGHTGRAY);
        
        EndDrawing();
    }

    // Unload all resources
    res.UnloadAll();

    CloseWindow();
    return 0;
}
