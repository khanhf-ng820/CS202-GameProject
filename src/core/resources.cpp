#include "resources.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

namespace {
    // Helper to extract tag value from a string
    std::string ExtractTag(const std::string& str, const std::string& tag) {
        std::string startTag = "<" + tag + ">";
        std::string endTag = "</" + tag + ">";
        size_t start = str.find(startTag);
        if (start == std::string::npos) return "";
        size_t end = str.find(endTag, start + startTag.length());
        if (end == std::string::npos) return "";
        return str.substr(start + startTag.length(), end - start - startTag.length());
    }

    // Convert string to uppercase
    std::string ToUpper(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
        return s;
    }

    // Extract file stem (name without extension)
    std::string GetFileStem(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        std::string filename = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
        size_t lastDot = filename.find_last_of('.');
        if (lastDot == std::string::npos) return filename;
        return filename.substr(0, lastDot);
    }

    // Normalize backslashes to forward slashes for consistent map keys
    std::string NormalizePath(std::string path) {
        for (char& c : path) {
            if (c == '\\') c = '/';
        }
        return path;
    }
}

Resources& Resources::GetInstance() {
    static Resources instance;
    return instance;
}

void Resources::LoadFile(const std::string& path) {
    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos) return;

    std::string ext = ToUpper(path.substr(lastDot));
    if (ext == ".PNG" || ext == ".JPG" || ext == ".JPEG") {
        std::string stem = GetFileStem(path);
        if (!stem.empty() && stem.back() == '_') {
            return;
        }
        if (stem == "ZombieNoteHelp") {
            return; // Skip loading as standalone texture since it serves as the alpha mask for ZombieNoteHelpBlack
        }

        std::string key = ToUpper(stem);
        if (textures.find(key) != textures.end()) {
            return; // Already loaded
        }

        Image img = LoadImage(path.c_str());
        if (img.data != nullptr) {
            std::string basePathWithoutExt = path.substr(0, lastDot);
            std::string maskPng = basePathWithoutExt + "_.png";
            std::string maskJpg = basePathWithoutExt + "_.jpg";
            std::string maskPath = "";

            if (FileExists(maskPng.c_str())) {
                maskPath = maskPng;
            } else if (FileExists(maskJpg.c_str())) {
                maskPath = maskJpg;
            } else if (key == "ZOMBIENOTEHELPBLACK") {
                size_t lastSlash = path.find_last_of("/\\");
                std::string dir = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : ".";
                std::string customMask = dir + "/ZombieNoteHelp.png";
                if (FileExists(customMask.c_str())) {
                    maskPath = customMask;
                }
            }

            if (!maskPath.empty()) {
                Image alphaMask = LoadImage(maskPath.c_str());
                if (alphaMask.data != nullptr) {
                    ImageAlphaMask(&img, alphaMask);
                    UnloadImage(alphaMask);
                }
            }

            Texture2D tex = LoadTextureFromImage(img);
            if (tex.id != 0) {
                SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
                textures[key] = tex;
                images[key] = img;
            } else {
                UnloadImage(img);
            }
        }
    } else if (ext == ".REANIM") {
        LoadReanim(path);
    }
}

void Resources::LoadMinimalForLoadingScreen() {
    if (background.id == 0) {
        std::string bgPath = GetAssetPath("assets/images/background1.png");
        background = LoadTexture(bgPath.c_str());
        if (background.id != 0) {
            GenTextureMipmaps(&background);
            SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);
        }
    }

    std::string titlePath = GetAssetPath("assets/images/titlescreen.jpg");
    LoadFile(titlePath);

    std::string logoPath = GetAssetPath("assets/images/PvZ_Logo.jpg");
    LoadFile(logoPath);

    std::string dirtPath = GetAssetPath("assets/images/LoadBar_dirt.png");
    LoadFile(dirtPath);

    std::string grassPath = GetAssetPath("assets/images/LoadBar_grass.png");
    LoadFile(grassPath);

    std::string sodCapPath = GetAssetPath("assets/reanim/SodRollCap.png");
    LoadFile(sodCapPath);
}

void Resources::PrepareAssetLoadingQueue(const std::vector<std::string>& dirPaths) {
    m_pendingFileQueue.clear();
    m_loadedAssetCount = 0;

    for (const auto& dirPath : dirPaths) {
        std::string resolvedDir = GetAssetPath(dirPath);
        if (!DirectoryExists(resolvedDir.c_str())) continue;

        FilePathList files = LoadDirectoryFiles(resolvedDir.c_str());
        for (unsigned int i = 0; i < files.count; i++) {
            std::string path = files.paths[i];
            size_t lastDot = path.find_last_of('.');
            if (lastDot != std::string::npos) {
                std::string ext = ToUpper(path.substr(lastDot));
                if (ext == ".PNG" || ext == ".JPG" || ext == ".JPEG" || ext == ".REANIM") {
                    std::string stem = GetFileStem(path);
                    if (!stem.empty() && stem.back() == '_') {
                        continue;
                    }
                    m_pendingFileQueue.push_back(path);
                }
            }
        }
        UnloadDirectoryFiles(files);
    }

    m_totalAssetCount = (int)m_pendingFileQueue.size();
}

bool Resources::StepAssetLoadingQueue(int batchSize) {
    for (int i = 0; i < batchSize; ++i) {
        if (m_pendingFileQueue.empty()) {
            return true;
        }
        std::string nextFile = m_pendingFileQueue.back();
        m_pendingFileQueue.pop_back();

        LoadFile(nextFile);
        m_loadedAssetCount++;
    }

    return m_pendingFileQueue.empty();
}

float Resources::GetLoadingProgress() const {
    if (m_totalAssetCount <= 0) return 1.0f;
    float progress = (float)m_loadedAssetCount / (float)m_totalAssetCount;
    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;
    return progress;
}

void Resources::LoadAll(const std::string& filePath) {
    if (background.id == 0) {
        std::string bgPath = GetAssetPath("assets/images/background1.png");
        background = LoadTexture(bgPath.c_str());
        if (background.id != 0) {
            GenTextureMipmaps(&background);
            SetTextureFilter(background, TEXTURE_FILTER_BILINEAR);
        }
    }

    FilePathList files = LoadDirectoryFiles(filePath.c_str());
    for (unsigned int i = 0; i < files.count; i++) {
        LoadFile(files.paths[i]);
    }
    UnloadDirectoryFiles(files);
}

void Resources::UnloadAll() {
    if (background.id != 0) {
        UnloadTexture(background);
        background = {0};
    }
    for (auto const& pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();
    for (auto& pair : images) {
        UnloadImage(pair.second);
    }
    images.clear();
}

Texture2D Resources::GetTexture(const std::string& name) const {
    const char* start = name.c_str();
    if (strncmp(start, "IMAGE_REANIM_", 13) == 0) {
        start += 13;
    } else if (strncmp(start, "IMAGE_", 6) == 0) {
        start += 6;
    }

    char upperKey[128];
    size_t i = 0;
    while (start[i] != '\0' && i < 127) {
        upperKey[i] = toupper((unsigned char)start[i]);
        i++;
    }
    upperKey[i] = '\0';

    std::string key(upperKey);

    // Redirect background images to their transparent versions if they exist
    if (key == "SELECTORSCREEN_BG_LEFT" || key == "SELECTORSCREEN_BG_CENTER" || key == "SELECTORSCREEN_BG_RIGHT" || key == "OPTIONS_MENUBACK") {
        std::string transKey = key + "_TRANSPARENT";
        auto itTrans = textures.find(transKey);
        if (itTrans != textures.end()) {
            return itTrans->second;
        }
    }

    auto it = textures.find(key);
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{0};
}

bool Resources::IsPixelTransparent(const std::string& name, int x, int y) const {
    std::string key = name;
    // Strip IMAGE_REANIM_
    if (key.rfind("IMAGE_REANIM_", 0) == 0) {
        key = key.substr(13);
    }
    // Strip IMAGE_
    else if (key.rfind("IMAGE_", 0) == 0) {
        key = key.substr(6);
    }
    key = ToUpper(key);

    auto it = images.find(key);
    if (it != images.end()) {
        const Image& img = it->second;
        if (x >= 0 && x < img.width && y >= 0 && y < img.height) {
            Color color = GetImageColor(img, x, y);
            return color.a == 0;
        }
    }
    return true; // Out of bounds or not found counts as transparent (not clickable)
}

std::string Resources::GetAssetPath(const std::string& relativePath) {
    std::string norm = NormalizePath(relativePath);
    if (FileExists(norm.c_str()) || DirectoryExists(norm.c_str())) {
        return norm;
    }
    std::string path = "../" + norm;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return NormalizePath(path);
    }
    path = "../../" + norm;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return NormalizePath(path);
    }
    path = "../../../" + norm;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return NormalizePath(path);
    }
    return norm; // Fallback
}

std::string Resources::FormatAnimName(const std::string& raw) const {
    if (raw.rfind("anim_", 0) == 0) {
        std::string s = raw.substr(5);
        if (!s.empty()) {
            s[0] = std::toupper(s[0]);
        }
        return s;
    }
    return raw;
}

ReanimDefinition Resources::LoadReanim(const std::string& filePath) {
    std::string normPath = NormalizePath(filePath);
    auto it = reanims.find(normPath);
    if (it != reanims.end()) {
        return it->second;
    }

    ReanimDefinition def;
    std::ifstream file(normPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open reanim file: " << normPath << std::endl;
        return def;
    }

    std::string line;
    ReanimTrack* currentTrack = nullptr;

    while (std::getline(file, line)) {
        // Parse FPS
        if (line.find("<fps>") != std::string::npos) {
            std::string fpsStr = ExtractTag(line, "fps");
            if (!fpsStr.empty()) {
                def.fps = std::stof(fpsStr);
            }
            continue;
        }

        // Parse track start
        if (line.find("<track>") != std::string::npos) {
            def.tracks.emplace_back();
            currentTrack = &def.tracks.back();
            continue;
        }

        // Parse track name
        if (currentTrack && line.find("<name>") != std::string::npos) {
            currentTrack->name = ExtractTag(line, "name");
            continue;
        }

        // Parse keyframe <t>
        if (currentTrack && line.find("<t>") != std::string::npos) {
            ReanimKeyframe kf;
            
            // Get previous keyframe for inheritance
            if (!currentTrack->keyframes.empty()) {
                kf = currentTrack->keyframes.back();
            } else {
                // Default values for first frame
                kf.x = 0.0f;
                kf.y = 0.0f;
                kf.sx = 1.0f;
                kf.sy = 1.0f;
                kf.kx = 0.0f;
                kf.ky = 0.0f;
                kf.f = 0;
                kf.imageName = "";
            }

            // Extract values if tags present on this line
            std::string xStr = ExtractTag(line, "x");
            if (!xStr.empty()) kf.x = std::stof(xStr);

            std::string yStr = ExtractTag(line, "y");
            if (!yStr.empty()) kf.y = std::stof(yStr);

            std::string sxStr = ExtractTag(line, "sx");
            if (!sxStr.empty()) kf.sx = std::stof(sxStr);

            std::string syStr = ExtractTag(line, "sy");
            if (!syStr.empty()) kf.sy = std::stof(syStr);

            std::string kxStr = ExtractTag(line, "kx");
            if (!kxStr.empty()) kf.kx = std::stof(kxStr);

            std::string kyStr = ExtractTag(line, "ky");
            if (!kyStr.empty()) kf.ky = std::stof(kyStr);

            std::string fStr = ExtractTag(line, "f");
            if (!fStr.empty()) kf.f = std::stoi(fStr);

            std::string iStr = ExtractTag(line, "i");
            if (!iStr.empty()) kf.imageName = iStr;

            currentTrack->keyframes.push_back(kf);
        }
    }

    // Determine max frames
    for (const auto& track : def.tracks) {
        if ((int)track.keyframes.size() > def.maxFrames) {
            def.maxFrames = (int)track.keyframes.size();
        }
    }

    reanims[normPath] = def;
    return def;
}