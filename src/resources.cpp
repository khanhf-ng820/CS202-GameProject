#include "resources.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

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
}

Resources& Resources::GetInstance() {
    static Resources instance;
    return instance;
}

void Resources::LoadAll(const std::string& filePath) {
    // Load background
    std::string bgPath = GetAssetPath("assets/images/background1.png");
    background = LoadTexture(bgPath.c_str());
    if (background.id == 0) {
        std::cerr << "Warning: Failed to load background from " << bgPath << std::endl;
    }

    // Load textures
    FilePathList files = LoadDirectoryFiles(filePath.c_str());
    for (unsigned int i = 0; i < files.count; i++) {
        std::string path = files.paths[i];
        // Check if extension is .png, .jpg, or .jpeg
        size_t lastDot = path.find_last_of('.');
        if (lastDot != std::string::npos) {
            std::string ext = ToUpper(path.substr(lastDot));
            if (ext == ".PNG" || ext == ".JPG" || ext == ".JPEG") {
                std::string stem = GetFileStem(path);
                std::string key = ToUpper(stem);

                Texture2D tex = LoadTexture(path.c_str());
                if (tex.id != 0) {
                    textures[key] = tex;
                }
            }
        }
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
}

Texture2D Resources::GetTexture(const std::string& name) const {
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

std::string Resources::GetAssetPath(const std::string& relativePath) {
    if (FileExists(relativePath.c_str()) || DirectoryExists(relativePath.c_str())) {
        return relativePath;
    }
    std::string path = "../" + relativePath;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return path;
    }
    path = "../../" + relativePath;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return path;
    }
    path = "../../../" + relativePath;
    if (FileExists(path.c_str()) || DirectoryExists(path.c_str())) {
        return path;
    }
    return relativePath; // Fallback
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
    ReanimDefinition def;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open reanim file: " << filePath << std::endl;
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

    return def;
}