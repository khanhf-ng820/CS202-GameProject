#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"
#include <string>

class LoadingScreen {
public:
    LoadingScreen(Resources& res);
    ~LoadingScreen() = default;

    // Updates loading progress by processing asset queue batches. Returns true when done.
    bool update(float dt);

    // Renders the loading screen graphics onto the 800x600 virtual canvas.
    void draw();

private:
    Resources& m_res;
    Texture2D m_titleTex;
    Texture2D m_logoTex;
    Texture2D m_dirtTex;
    Texture2D m_grassTex;
    Texture2D m_sodCapTex;
    BitmapFont m_font;
    bool m_fontLoaded = false;
    float m_rollRotation = 0.0f;
    bool m_isFinished = false;
};
