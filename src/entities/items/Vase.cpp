#include "Vase.h"

Vase::Vase(int row, int col, float x, float y, VaseType type, VaseContent content)
    : m_row(row), m_col(col), m_x(x), m_y(y), m_type(type), m_state(VaseState::Intact), m_content(content) {
}

void Vase::draw(Resources& res) const {
    if (m_state == VaseState::Destroyed) return;

    Texture2D potTex = res.GetTexture("SCARY_POT");
    if (potTex.id == 0) potTex = res.GetTexture("Scary_Pot");
    if (potTex.id == 0) {
        std::string potPath = res.GetAssetPath("assets/images/Scary_Pot.png");
        res.LoadFile(potPath);
        potTex = res.GetTexture("SCARY_POT");
        if (potTex.id == 0) potTex = res.GetTexture("Scary_Pot");
    }

    if (potTex.id != 0) {
        float colIndex = 0.0f;
        if (m_type == VaseType::Green) {
            colIndex = 1.0f;
        } else if (m_type == VaseType::Zombie) {
            colIndex = 2.0f;
        }

        // Draw Row 2 textures (y = 101.0f, height = 101.0f)
        Rectangle srcRec = { colIndex * 80.0f, 101.0f, 80.0f, 101.0f };
        Rectangle destRec = { m_x, m_y, 80.0f, 101.0f };
        DrawTexturePro(potTex, srcRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        // Fallback placeholder rectangle
        Color fallbackColor = (m_type == VaseType::Green) ? GREEN : BROWN;
        DrawRectangleRec({ m_x + 10.0f, m_y + 10.0f, 60.0f, 80.0f }, fallbackColor);
    }
}

BrownVase::BrownVase(int row, int col, float x, float y, VaseContent content)
    : Vase(row, col, x, y, VaseType::Brown, content) {
}

GreenVase::GreenVase(int row, int col, float x, float y, VaseContent content)
    : Vase(row, col, x, y, VaseType::Green, content) {
}
