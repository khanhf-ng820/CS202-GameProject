#pragma once
#include "raylib.h"
#include "reanim.h"
#include <string>
#include <vector>
#include <unordered_map>

class Resources {
public:
    static Resources& GetInstance();

    void LoadAll(const std::string& filePath);
    void UnloadAll();

    std::string GetAssetPath(const std::string& relativePath);
    Texture2D GetBackground() const { return background; }
    Texture2D GetTexture(const std::string& name) const;
    bool IsPixelTransparent(const std::string& name, int x, int y) const;
    std::string FormatAnimName(const std::string& raw) const;
    ReanimDefinition LoadReanim(const std::string& filePath);

private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Image> images;
    Texture2D background;

    Resources() = default;
    ~Resources() = default;
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;
};

