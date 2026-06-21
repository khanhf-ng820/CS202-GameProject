#pragma once
#include <raylib.h>
#include <string>
#include <vector>

struct ReanimKeyframe {
    float x = 0.0f;
    float y = 0.0f;
    float sx = 1.0f;
    float sy = 1.0f;
    float kx = 0.0f;
    float ky = 0.0f;
    int f = 0; // -1 means inactive/hidden
    std::string imageName;
};

struct ReanimTrack {
    std::string name;
    std::vector<ReanimKeyframe> keyframes;
};

struct ReanimDefinition {
    float fps = 12.0f;
    std::vector<ReanimTrack> tracks;
    int maxFrames = 0;
};

struct AnimationRange {
    std::string name;
    int startFrame = 0;
    int endFrame = 0;
};