#include "UIHelpers.h"

static float g_virtualMouseScaleX = 1.0f;
static float g_virtualMouseScaleY = 1.0f;
static bool g_uiInteractionEnabled = true;

void SetVirtualMouseScale(float scaleX, float scaleY) {
    g_virtualMouseScaleX = scaleX;
    g_virtualMouseScaleY = scaleY;
}

Vector2 GetVirtualMousePosition() {
    Vector2 rawMouse = GetMousePosition();
    return { rawMouse.x * g_virtualMouseScaleX, rawMouse.y * g_virtualMouseScaleY };
}

void SetUIInteractionEnabled(bool enabled) {
    g_uiInteractionEnabled = enabled;
}

bool IsUIInteractionEnabled() {
    return g_uiInteractionEnabled;
}

// Draw a simple button and return true if clicked
bool DrawButton(Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor) {
    if (!g_uiInteractionEnabled) {
        DrawRectangleRec(rect, baseColor);
        DrawRectangleLinesEx(rect, 2.0f, ColorAlpha(textColor, 0.5f));
        int fontSize = 18;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - fontSize)/2, fontSize, textColor);
        return false;
    }

    Vector2 mousePos = GetVirtualMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);
    Color col = hovered ? hoverColor : baseColor;
    
    DrawRectangleRec(rect, col);
    DrawRectangleLinesEx(rect, 2.0f, ColorAlpha(textColor, 0.5f));
    
    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - fontSize)/2, fontSize, textColor);
    
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
