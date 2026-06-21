#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
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
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
//#pragma once 

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
    std::string particlesDir = res.GetAssetPath("assets/particles");
    res.LoadAll(particlesDir);

    int currentPlantType = 3; // Default to Fire Pea for testing
    std::unique_ptr<Plant> currentPlant = std::make_unique<FirePea>(res, 550, 420);
    std::vector<Projectile> projectiles;
    std::vector<SunItem> suns;


    while (!WindowShouldClose()) {
        // --- Update ---
        float dt = GetFrameTime();
        
        if (!currentPlant->isDead()) {
            currentPlant->update(dt, projectiles, suns);
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

        if (!currentPlant->isDead()) {
            currentPlant->draw();
        } else {
            if (DrawButton({ 530, 360, 200, 50 }, "RESPAWN PLANT", RED, MAROON, WHITE)) {
                if (currentPlantType == 0) currentPlant = std::make_unique<PeaShooter>(res, 550, 420);
                else if (currentPlantType == 1) currentPlant = std::make_unique<SnowPea>(res, 550, 420);
                else if (currentPlantType == 2) currentPlant = std::make_unique<Cornpult>(res, 550, 420);
                else if (currentPlantType == 3) currentPlant = std::make_unique<FirePea>(res, 550, 420);
                else if (currentPlantType == 4) currentPlant = std::make_unique<Jalapeno>(res, 550, 420);
                else if (currentPlantType == 5) currentPlant = std::make_unique<SunFlower>(res, 550, 420);
                else if (currentPlantType == 6) currentPlant = std::make_unique<GatlingPea>(res, 550, 420);
                else if (currentPlantType == 7) currentPlant = std::make_unique<Repeater>(res, 550, 420);
                else if (currentPlantType == 8) currentPlant = std::make_unique<Melonpult>(res, 550, 420);
                else if (currentPlantType == 9) currentPlant = std::make_unique<Wallnut>(res, 550, 420);
                else if (currentPlantType == 10) currentPlant = std::make_unique<CherryBomb>(res, 550, 420);
                else if (currentPlantType == 11) currentPlant = std::make_unique<Chomper>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        
        for (const auto& p : projectiles) {
            p.draw();
        }

        for (const auto& s : suns) {
            s.draw();
        }

        // 3. Draw Controls UI Panel (Glassmorphism overlay on the left)
        DrawRectangleRec({ 0, 0, 320, (float)GetScreenHeight() }, ColorAlpha(DARKBLUE, 0.75f));
        DrawRectangleLines(0, 0, 320, GetScreenHeight(), ColorAlpha(WHITE, 0.3f));

        // Title
        DrawText("PLANT REANIMATOR", 20, 20, 22, SKYBLUE);
        DrawText("C++ & Raylib Visualizer (OOP)", 20, 48, 14, GRAY);
        DrawLine(20, 72, 300, 72, ColorAlpha(WHITE, 0.2f));

        // Plant selector section
        DrawText("Select Plant Type:", 20, 85, 16, SKYBLUE);
        
        if (DrawButton({ 20, 115, 135, 30 }, "Peashooter", currentPlantType == 0 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 0 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 0) {
                currentPlantType = 0;
                currentPlant = std::make_unique<PeaShooter>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 115, 135, 30 }, "Snow Pea", currentPlantType == 1 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 1 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 1) {
                currentPlantType = 1;
                currentPlant = std::make_unique<SnowPea>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 20, 150, 135, 30 }, "Cornpult", currentPlantType == 2 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 2 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 2) {
                currentPlantType = 2;
                currentPlant = std::make_unique<Cornpult>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 150, 135, 30 }, "Fire Pea", currentPlantType == 3 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 3 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 3) {
                currentPlantType = 3;
                currentPlant = std::make_unique<FirePea>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 20, 185, 135, 30 }, "Jalapeno", currentPlantType == 4 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 4 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 4) {
                currentPlantType = 4;
                currentPlant = std::make_unique<Jalapeno>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 185, 135, 30 }, "Sunflower", currentPlantType == 5 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 5 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 5) {
                currentPlantType = 5;
                currentPlant = std::make_unique<SunFlower>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 20, 220, 135, 30 }, "Gatling Pea", currentPlantType == 6 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 6 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 6) {
                currentPlantType = 6;
                currentPlant = std::make_unique<GatlingPea>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 220, 135, 30 }, "Repeater", currentPlantType == 7 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 7 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 7) {
                currentPlantType = 7;
                currentPlant = std::make_unique<Repeater>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 20, 255, 135, 30 }, "Melon Pult", currentPlantType == 8 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 8 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 8) {
                currentPlantType = 8;
                currentPlant = std::make_unique<Melonpult>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 255, 135, 30 }, "Wallnut", currentPlantType == 9 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 9 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 9) {
                currentPlantType = 9;
                currentPlant = std::make_unique<Wallnut>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 20, 290, 135, 30 }, "Cherry Bomb", currentPlantType == 10 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 10 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 10) {
                currentPlantType = 10;
                currentPlant = std::make_unique<CherryBomb>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }
        if (DrawButton({ 165, 290, 135, 30 }, "Chomper", currentPlantType == 11 ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f), currentPlantType == 11 ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f), WHITE)) {
            if (currentPlantType != 11) {
                currentPlantType = 11;
                currentPlant = std::make_unique<Chomper>(res, 550, 420);
                projectiles.clear();
                suns.clear();
            }
        }

        DrawLine(20, 330, 300, 330, ColorAlpha(WHITE, 0.2f));
        DrawLine(20, 400, 300, 400, ColorAlpha(WHITE, 0.2f));

        // Animations list header
        DrawText("Select Animation:", 20, 415, 16, SKYBLUE);

        // Draw a list of animation buttons
        int startY = 445;
        const auto& anims = currentPlant->getAnim().GetAnimations();
        for (size_t i = 0; i < anims.size(); ++i) {
            std::string label = res.FormatAnimName(anims[i].name);
            bool isCurrent = ((int)i == currentPlant->getAnim().GetCurrentAnimIndex());
            Color baseCol = isCurrent ? ColorAlpha(GREEN, 0.6f) : ColorAlpha(DARKGRAY, 0.3f);
            Color hoverCol = isCurrent ? ColorAlpha(GREEN, 0.8f) : ColorAlpha(GRAY, 0.6f);

            // Break if we exceed screen bounds
            if (startY + 35 > GetScreenHeight() - 50) {
                DrawText("...", 20, startY, 14, GRAY);
                break;
            }

            if (DrawButton({ 20, (float)startY, 280, 30 }, label.c_str(), baseCol, hoverCol, WHITE)) {
                currentPlant->getAnim().SetAnimationIndex((int)i);
            }
            startY += 35;
        }

        // Footer instructions
        DrawRectangleRec({ 340, 20, 450, 40 }, ColorAlpha(BLACK, 0.5f));
        DrawText("Click buttons on the left to switch animation/plant type.", 355, 32, 14, LIGHTGRAY);
        
        EndDrawing();
    }

    // Unload all resources
    res.UnloadAll();

    CloseWindow();
    return 0;
}
