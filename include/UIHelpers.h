#pragma once
#include "raylib.h"

// Draw a simple button and return true if clicked
bool DrawButton(Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor);

// Support for virtual resolution mouse scaling
void SetVirtualMouseScale(float scaleX, float scaleY);
Vector2 GetVirtualMousePosition();

// Control whether UI buttons are interactive
void SetUIInteractionEnabled(bool enabled);
bool IsUIInteractionEnabled();


