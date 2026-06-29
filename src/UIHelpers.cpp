#include "UIHelpers.h"

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
