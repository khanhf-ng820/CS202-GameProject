#include "BitmapFont.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>

// ---------------------------------------------------------------------------
// Helper: trim whitespace from both ends
// ---------------------------------------------------------------------------
static std::string Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ---------------------------------------------------------------------------
// Helper: parse a parenthesised list of chars, e.g. ('A', 'B', ...)
// Returns only basic ASCII chars (skips multi-byte / invalid entries).
// ---------------------------------------------------------------------------
static std::vector<char> ParseCharList(const std::string& body) {
    std::vector<char> result;
    size_t pos = 0;
    while (pos < body.size()) {
        // Look for a single-quoted character
        size_t q1 = body.find('\'', pos);
        if (q1 == std::string::npos) break;
        size_t q2 = body.find('\'', q1 + 1);
        if (q2 == std::string::npos) break;

        std::string inside = body.substr(q1 + 1, q2 - q1 - 1);
        if (inside.size() == 1 && static_cast<unsigned char>(inside[0]) < 128) {
            result.push_back(inside[0]);
        } else {
            // Non-ASCII or multi-byte: push a placeholder so indices stay aligned
            result.push_back('\0');
        }

        // Also handle escaped quote inside quotes — but PvZ fonts don't use this,
        // and the descriptor uses "'" (double-quoted) for the apostrophe entry.
        pos = q2 + 1;
    }

    // Check for double-quoted entries like "'" which represents the apostrophe
    // The descriptor has: "'", '"' patterns
    // Actually let's also scan for double-quoted single chars
    pos = 0;
    size_t idx = 0;
    while (pos < body.size() && idx < result.size()) {
        size_t q1 = body.find('\'', pos);
        size_t dq1 = body.find('"', pos);

        if (dq1 != std::string::npos && (q1 == std::string::npos || dq1 < q1)) {
            // Found a double-quoted entry before the next single-quoted one
            size_t dq2 = body.find('"', dq1 + 1);
            if (dq2 != std::string::npos) {
                std::string inside = body.substr(dq1 + 1, dq2 - dq1 - 1);
                if (inside.size() == 1 && static_cast<unsigned char>(inside[0]) < 128) {
                    // This is a valid char — but we already parsed via single quotes
                    // so this might be the apostrophe "'" entry
                }
                pos = dq2 + 1;
            } else {
                break;
            }
        } else if (q1 != std::string::npos) {
            size_t q2 = body.find('\'', q1 + 1);
            if (q2 == std::string::npos) break;
            pos = q2 + 1;
            idx++;
        } else {
            break;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Helper: parse a parenthesised list of ints, e.g. ( 22, 18, 19, ... )
// ---------------------------------------------------------------------------
static std::vector<int> ParseIntList(const std::string& body) {
    std::vector<int> result;
    std::string clean;
    for (char c : body) {
        if (c == '(' || c == ')') continue;
        clean += c;
    }
    std::istringstream iss(clean);
    std::string token;
    while (std::getline(iss, token, ',')) {
        token = Trim(token);
        if (!token.empty()) {
            try {
                result.push_back(std::stoi(token));
            } catch (...) {
                result.push_back(0);
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Helper: parse a list of (x, y, w, h) tuples
// ---------------------------------------------------------------------------
static std::vector<Rectangle> ParseRectList(const std::string& body) {
    std::vector<Rectangle> result;
    size_t pos = 0;
    while (pos < body.size()) {
        size_t open = body.find('(', pos);
        if (open == std::string::npos) break;
        size_t close = body.find(')', open);
        if (close == std::string::npos) break;

        std::string inner = body.substr(open + 1, close - open - 1);
        std::vector<int> vals = ParseIntList("(" + inner + ")");
        if (vals.size() >= 4) {
            result.push_back({(float)vals[0], (float)vals[1], (float)vals[2], (float)vals[3]});
        }
        pos = close + 1;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Helper: parse a list of (x, y) offset tuples
// ---------------------------------------------------------------------------
static std::vector<Vector2> ParseOffsetList(const std::string& body) {
    std::vector<Vector2> result;
    size_t pos = 0;
    while (pos < body.size()) {
        size_t open = body.find('(', pos);
        if (open == std::string::npos) break;
        size_t close = body.find(')', open);
        if (close == std::string::npos) break;

        std::string inner = body.substr(open + 1, close - open - 1);
        std::vector<int> vals = ParseIntList("(" + inner + ")");
        if (vals.size() >= 2) {
            result.push_back({(float)vals[0], (float)vals[1]});
        }
        pos = close + 1;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Helper: parse kerning pair strings, e.g. ("AV","AW", ...)
// ---------------------------------------------------------------------------
static std::vector<std::string> ParseStringList(const std::string& body) {
    std::vector<std::string> result;
    size_t pos = 0;
    while (pos < body.size()) {
        size_t q1 = body.find('"', pos);
        if (q1 == std::string::npos) break;
        size_t q2 = body.find('"', q1 + 1);
        if (q2 == std::string::npos) break;
        result.push_back(body.substr(q1 + 1, q2 - q1 - 1));
        pos = q2 + 1;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Helper: read the entire body following a "Define <Name>" block.
// The body spans from the next line after "Define <Name>" until the next
// "Define" or non-continuation line.
// ---------------------------------------------------------------------------
static std::string ReadDefineBody(std::ifstream& file) {
    std::string body;
    std::string line;
    std::streampos prevPos = file.tellg();
    while (std::getline(file, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty()) {
            prevPos = file.tellg();
            continue;
        }
        // If the line starts with a new command keyword, we've gone too far
        if (trimmed.rfind("Define", 0) == 0 ||
            trimmed.rfind("CreateLayer", 0) == 0 ||
            trimmed.rfind("LayerSet", 0) == 0 ||
            trimmed.rfind("SetDefault", 0) == 0) {
            file.seekg(prevPos);
            break;
        }
        body += " " + trimmed;
        prevPos = file.tellg();
    }
    return body;
}

// ---------------------------------------------------------------------------
// BitmapFont::Load
// ---------------------------------------------------------------------------
bool BitmapFont::Load(const std::string& pngPath, const std::string& txtPath) {
    // Load the atlas texture
    m_atlas = LoadTexture(pngPath.c_str());
    if (m_atlas.id == 0) {
        std::cerr << "BitmapFont: Failed to load atlas: " << pngPath << std::endl;
        return false;
    }

    // Parse the descriptor file
    std::ifstream file(txtPath);
    if (!file.is_open()) {
        std::cerr << "BitmapFont: Failed to open descriptor: " << txtPath << std::endl;
        return false;
    }

    std::vector<char> charList;
    std::vector<int> widthList;
    std::vector<Rectangle> rectList;
    std::vector<Vector2> offsetList;
    std::vector<std::string> kerningPairs;
    std::vector<int> kerningValues;

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty()) continue;

        if (trimmed.rfind("Define CharList", 0) == 0) {
            std::string body = ReadDefineBody(file);
            charList = ParseCharList(body);
        }
        else if (trimmed.rfind("Define WidthList", 0) == 0) {
            std::string body = ReadDefineBody(file);
            widthList = ParseIntList(body);
        }
        else if (trimmed.rfind("Define RectList", 0) == 0) {
            std::string body = ReadDefineBody(file);
            rectList = ParseRectList(body);
        }
        else if (trimmed.rfind("Define OffsetList", 0) == 0) {
            std::string body = ReadDefineBody(file);
            offsetList = ParseOffsetList(body);
        }
        else if (trimmed.rfind("Define KerningPairs", 0) == 0) {
            std::string body = ReadDefineBody(file);
            kerningPairs = ParseStringList(body);
        }
        else if (trimmed.rfind("Define KerningValues", 0) == 0) {
            std::string body = ReadDefineBody(file);
            kerningValues = ParseIntList(body);
        }
    }

    // Build the glyph map (ASCII only — skip null placeholders)
    size_t count = std::min({charList.size(), widthList.size(), rectList.size(), offsetList.size()});
    for (size_t i = 0; i < count; ++i) {
        char ch = charList[i];
        if (ch == '\0') continue;  // Skip non-ASCII placeholders
        GlyphInfo glyph;
        glyph.srcRect = rectList[i];
        glyph.advanceWidth = widthList[i];
        glyph.offset = offsetList[i];
        m_glyphs[ch] = glyph;

        // Track native glyph height from first valid rect
        if (m_glyphHeight == 0 && glyph.srcRect.height > 0) {
            m_glyphHeight = (int)glyph.srcRect.height;
        }
    }

    // Also add space if not already present
    if (m_glyphs.find(' ') == m_glyphs.end()) {
        GlyphInfo space;
        space.srcRect = {0, 0, 0, 0};
        space.advanceWidth = 14;  // From the descriptor: LayerSetCharWidths Main (' ') (14)
        space.offset = {0, 0};
        m_glyphs[' '] = space;
    }

    // Build the kerning map
    size_t kernCount = std::min(kerningPairs.size(), kerningValues.size());
    for (size_t i = 0; i < kernCount; ++i) {
        m_kerning[kerningPairs[i]] = kerningValues[i];
    }

    std::cout << "BitmapFont: Loaded " << m_glyphs.size() << " glyphs, "
              << m_kerning.size() << " kerning pairs" << std::endl;

    return true;
}

// ---------------------------------------------------------------------------
// BitmapFont::Unload
// ---------------------------------------------------------------------------
void BitmapFont::Unload() {
    if (m_atlas.id != 0) {
        UnloadTexture(m_atlas);
        m_atlas = {0};
    }
    m_glyphs.clear();
    m_kerning.clear();
}

// ---------------------------------------------------------------------------
// BitmapFont::MeasureText
// ---------------------------------------------------------------------------
int BitmapFont::MeasureText(const char* text, float scale) const {
    if (!text) return 0;

    float totalWidth = 0.0f;
    size_t len = std::strlen(text);

    for (size_t i = 0; i < len; ++i) {
        char ch = text[i];
        auto it = m_glyphs.find(ch);
        if (it == m_glyphs.end()) continue;

        totalWidth += (float)it->second.advanceWidth;

        // Apply kerning if there's a next character
        if (i + 1 < len) {
            std::string pair = {ch, text[i + 1]};
            auto kit = m_kerning.find(pair);
            if (kit != m_kerning.end()) {
                totalWidth += (float)kit->second;
            }
        }
    }

    return (int)(totalWidth * scale);
}

// ---------------------------------------------------------------------------
// BitmapFont::DrawText
// ---------------------------------------------------------------------------
void BitmapFont::DrawText(const char* text, float x, float y, float scale, Color tint) const {
    if (!text || m_atlas.id == 0) return;

    float cursorX = x;
    size_t len = std::strlen(text);

    for (size_t i = 0; i < len; ++i) {
        char ch = text[i];
        auto it = m_glyphs.find(ch);
        if (it == m_glyphs.end()) continue;

        const GlyphInfo& glyph = it->second;

        // Only draw if the glyph has a valid source rect (space has zero-size rect)
        if (glyph.srcRect.width > 0 && glyph.srcRect.height > 0) {
            Rectangle dest = {
                cursorX + glyph.offset.x * scale,
                y + glyph.offset.y * scale,
                glyph.srcRect.width * scale,
                glyph.srcRect.height * scale
            };
            DrawTexturePro(m_atlas, glyph.srcRect, dest, {0, 0}, 0.0f, tint);
        }

        cursorX += (float)glyph.advanceWidth * scale;

        // Apply kerning
        if (i + 1 < len) {
            std::string pair = {ch, text[i + 1]};
            auto kit = m_kerning.find(pair);
            if (kit != m_kerning.end()) {
                cursorX += (float)kit->second * scale;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// BitmapFont::DrawTextCentered
// ---------------------------------------------------------------------------
void BitmapFont::DrawTextCentered(const char* text, Rectangle bounds, float scale, Color tint) const {
    int textW = MeasureText(text, scale);
    float textH = (float)m_glyphHeight * scale;

    float x = bounds.x + (bounds.width - (float)textW) / 2.0f;
    float y = bounds.y + (bounds.height - textH) / 2.0f;

    DrawText(text, x, y, scale, tint);
}
