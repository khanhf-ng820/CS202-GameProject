#include "LoadingScreen.h"
#include <iostream>
#include <algorithm>

LoadingScreen::LoadingScreen(Resources& res)
    : m_res(res) {
    m_titleTex = res.GetTexture("TITLESCREEN");
    m_dirtTex  = res.GetTexture("LOADBAR_DIRT");
    m_grassTex = res.GetTexture("LOADBAR_GRASS");
    m_sodCapTex = res.GetTexture("SODROLLCAP");

    std::string fontPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string fontTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    if (FileExists(fontPng.c_str()) && FileExists(fontTxt.c_str())) {
        m_fontLoaded = m_font.Load(fontPng, fontTxt);
    }
}

bool LoadingScreen::update(float dt) {
    m_rollRotation += dt * 450.0f;
    if (m_rollRotation >= 360.0f) {
        m_rollRotation -= 360.0f;
    }

    // Step the loading queue (load 3 assets per frame)
    m_isFinished = m_res.StepAssetLoadingQueue(3);
    return m_isFinished;
}

void LoadingScreen::draw() {
    // 1. Draw Title Background (800x600 virtual canvas)
    if (m_titleTex.id != 0) {
        DrawTexturePro(
            m_titleTex,
            { 0.0f, 0.0f, (float)m_titleTex.width, (float)m_titleTex.height },
            { 0.0f, 0.0f, 800.0f, 600.0f },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        ClearBackground(BLACK);
    }

    // 2. Draw Loading Progress Bar
    float progress = m_res.GetLoadingProgress();

    float dirtX = 240.0f;
    float dirtY = 515.0f;

    if (m_dirtTex.id != 0) {
        DrawTexture(m_dirtTex, (int)dirtX, (int)dirtY, WHITE);
    }

    if (m_grassTex.id != 0 && progress > 0.0f) {
        float maxGrassW = (m_grassTex.width > 0) ? (float)m_grassTex.width : 314.0f;
        float grassW = maxGrassW * progress;
        float grassX = dirtX + 3.0f;
        float grassY = dirtY - (float)m_grassTex.height + 15.0f;

        Rectangle srcRect = { 0.0f, 0.0f, grassW, (float)m_grassTex.height };
        Rectangle destRect = { grassX, grassY, grassW, (float)m_grassTex.height };
        DrawTexturePro(m_grassTex, srcRect, destRect, { 0.0f, 0.0f }, 0.0f, WHITE);

        // Draw rolling Sod Cap at the leading edge of the grass
        if (m_sodCapTex.id != 0 && progress < 1.0f) {
            float capScale = 0.55f;
            float capW = (float)m_sodCapTex.width * capScale;
            float capH = (float)m_sodCapTex.height * capScale;
            float rollX = grassX + grassW - capW * 0.3f;
            float rollY = grassY + (float)m_grassTex.height * 0.5f;

            Rectangle rollSrc = { 0.0f, 0.0f, (float)m_sodCapTex.width, (float)m_sodCapTex.height };
            Rectangle rollDest = { rollX, rollY, capW, capH };
            Vector2 origin = { capW * 0.5f, capH * 0.5f };

            DrawTexturePro(m_sodCapTex, rollSrc, rollDest, origin, m_rollRotation, WHITE);
        }
    }

    // 3. Draw Percentage Text Overlay
    int percent = (int)(progress * 100.0f);
    if (percent > 100) percent = 100;
    std::string text = "LOADING... " + std::to_string(percent) + "%";

    Rectangle textBounds = { dirtX, dirtY + 5.0f, 321.0f, 40.0f };
    Rectangle shadowBounds = { dirtX + 1.5f, dirtY + 6.5f, 321.0f, 40.0f };

    if (m_fontLoaded) {
        m_font.DrawTextCentered(text.c_str(), shadowBounds, 0.65f, ColorAlpha(BLACK, 0.8f));
        m_font.DrawTextCentered(text.c_str(), textBounds, 0.65f, GOLD);
    } else {
        int textW = MeasureText(text.c_str(), 18);
        int textX = (int)(dirtX + (321.0f - textW) / 2.0f);
        int textY = (int)(dirtY + 16.0f);
        DrawText(text.c_str(), textX + 1, textY + 1, 18, BLACK);
        DrawText(text.c_str(), textX, textY, 18, GOLD);
    }
}
