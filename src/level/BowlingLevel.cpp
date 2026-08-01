#include "BowlingLevel.h"
#include "UIHelpers.h"
#include "AudioManager.h"

BowlingLevel::BowlingLevel(Resources& res, RenderTexture2D targetScreen)
    : res(res), targetScreen(targetScreen) {}

bool BowlingLevel::getGridCell(Vector2 mousePos, int& outRow, int& outCol) const {
    float startX = 140.0f;
    float startY = 80.0f;

    if (mousePos.x < startX || mousePos.y < startY) return false;

    int row = (int)((mousePos.y - startY) / 100.0f);
    if (row < 0 || row >= 5) return false;

    float relX = mousePos.x - startX;
    int col = (relX < 80.0f) ? 0 : 1 + (int)((relX - 80.0f) / 70.0f);

    if (col >= 0 && col < 9) {
        outRow = row;
        outCol = col;
        return true;
    }
    return false;
}

void BowlingLevel::update(float dt) {
    // Currently no dynamic gameplay state needed for lawn preview
}

void BowlingLevel::draw() {
    BeginTextureMode(targetScreen);
    ClearBackground(RAYWHITE);

    // 1. Draw Daytime Front Lawn Background
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
        // Fallback lawn grid rendering
        for (int y = 0; y < 600; y += 80) {
            for (int x = 0; x < 800; x += 80) {
                DrawRectangle(x, y, 80, 80, ((x / 80 + y / 80) % 2 == 0) ? GREEN : DARKGREEN);
            }
        }
    }

    // 2. Draw Red Wall-nut Bowling Stripe between 3rd and 4th columns (x = 360.0f) across all 5 lawn rows (y = 80 to 580)
    Texture2D stripeTex = res.GetTexture("WALLNUT_BOWLINGSTRIPE");
    if (stripeTex.id == 0) stripeTex = res.GetTexture("Wallnut_bowlingstripe");

    float stripeW = (stripeTex.id != 0) ? (float)stripeTex.width : 22.0f;
    float stripeH = 500.0f; // Spans all 5 rows (80.0f to 580.0f)
    float stripeX = 360.0f - (stripeW / 2.0f);
    float stripeY = 80.0f;

    if (stripeTex.id != 0) {
        DrawTexturePro(
            stripeTex,
            { 0.0f, 0.0f, (float)stripeTex.width, (float)stripeTex.height },
            { stripeX, stripeY, stripeW, stripeH },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    // 3. Draw hover highlight cell on front lawn grid
    Vector2 mousePos = GetVirtualMousePosition();
    int hoverRow, hoverCol;
    if (getGridCell(mousePos, hoverRow, hoverCol)) {
        float cellX = 140.0f + (hoverCol == 0 ? 0.0f : 80.0f + (hoverCol - 1) * 70.0f);
        float cellY = 80.0f + hoverRow * 100.0f;
        float cellW = (hoverCol == 0) ? 80.0f : 70.0f;
        float cellH = 100.0f;
        DrawRectangleLinesEx({ cellX, cellY, cellW, cellH }, 2.0f, ColorAlpha(GREEN, 0.6f));
    }

    EndTextureMode();

    // 3. Draw targetScreen stretched to actual window dimensions
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

void BowlingLevel::run() {
    SetUIInteractionEnabled(true);
    AudioManager::GetInstance().PlayMusic(MusicTrack::None);
    while (!WindowShouldClose()) {
        float scaleX = 800.0f / (float)GetScreenWidth();
        float scaleY = 600.0f / (float)GetScreenHeight();
        SetVirtualMouseScale(scaleX, scaleY);

        float dt = GetFrameTime();
        AudioManager::GetInstance().Update();
        update(dt);
        draw();

        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
    }
}
