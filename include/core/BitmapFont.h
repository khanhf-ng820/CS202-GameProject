#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// BitmapFont — loads and renders a PopCap-style bitmap font
//
// The font is defined by a PNG atlas and a TXT descriptor that specifies
// glyph source rectangles, advance widths, drawing offsets, and kerning.
// ---------------------------------------------------------------------------
class BitmapFont {
public:
    bool Load(const std::string& pngPath, const std::string& txtPath);
    void Unload();

    // Draw text at (x, y). scale = 1.0 uses the native glyph size (36px).
    void DrawText(const char* text, float x, float y, float scale, Color tint) const;

    // Draw text centered within a bounding rectangle.
    void DrawTextCentered(const char* text, Rectangle bounds, float scale, Color tint) const;

    // Measure the total pixel width of a string at the given scale.
    int MeasureText(const char* text, float scale) const;

private:
    Texture2D m_atlas{};

    struct GlyphInfo {
        Rectangle srcRect;    // Region in the atlas PNG
        int advanceWidth;     // Horizontal advance
        Vector2 offset;       // Drawing offset
    };

    std::unordered_map<char, GlyphInfo> m_glyphs;
    std::unordered_map<std::string, int> m_kerning;  // 2-char key -> kern value
    int m_glyphHeight = 36;  // Native glyph height from atlas
};
