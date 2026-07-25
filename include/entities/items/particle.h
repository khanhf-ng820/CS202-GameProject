#pragma once
#include "raylib.h"
#include <string>
#include "resources.h"

class ParticleEffect {
public:
    Texture2D texture{ 0 };
    float x{ 0.0f };
    float y{ 0.0f };
    float vx{ 0.0f };
    float vy{ 0.0f };
    float gravity{ 0.0f };
    float rotation{ 0.0f };
    float vr{ 0.0f };
    int currentFrame{ 0 };
    int totalFrames{ 1 };
    float frameDuration{ 0.05f };
    float timer{ 0.0f };
    float scale{ 1.0f };
    float alpha{ 1.0f };
    float fadeRate{ 0.0f };
    Color tint{ WHITE };
    bool loop{ false };
    bool active{ false };
    bool isPhysicsParticle{ false };

    ParticleEffect() = default;
    bool isActive() const { return active; }
    void setActive(bool isActive) { active = isActive; }
    void setTexture(Texture2D tex) { texture = tex; }
    void setTotalFrames(int totalFrames) { this->totalFrames = totalFrames; }
    void setFrameDuration(float frameDuration) { this->frameDuration = frameDuration; }
    void setScale(float scale) { this->scale = scale; }
    void setLoop(bool loop) { this->loop = loop; }

    void update(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void timing(float dt) {
        if (!active) return;

        if (isPhysicsParticle) {
            x += vx * dt;
            y += vy * dt;
            vy += gravity * dt;
            rotation += vr * dt;
            if (fadeRate > 0.0f) {
                alpha -= fadeRate * dt;
                if (alpha <= 0.0f) {
                    alpha = 0.0f;
                    active = false;
                }
            }
        }

        timer += dt;
        if (timer >= frameDuration) {
            timer = 0.0f;
            currentFrame++;
            if (currentFrame >= totalFrames) {
                if (loop) {
                    currentFrame = 0;
                } else if (!isPhysicsParticle) {
                    active = false;
                }
            }
        }
    }

    void draw() const {
        if (!active) return;
        Color drawTint = ColorAlpha(tint, alpha);

        if (texture.id != 0) {
            float frameW = (float)texture.width / (totalFrames > 0 ? totalFrames : 1);
            float frameH = (float)texture.height;
            Rectangle src = { currentFrame * frameW, 0.0f, frameW, frameH };
            Rectangle dest = { x, y, frameW * scale, frameH * scale };
            Vector2 origin = { (frameW * scale) / 2.0f, (frameH * scale) / 2.0f };
            DrawTexturePro(texture, src, dest, origin, rotation, drawTint);
        } else if (isPhysicsParticle) {
            Rectangle dest = { x, y, 6.0f * scale, 6.0f * scale };
            Vector2 origin = { 3.0f * scale, 3.0f * scale };
            DrawRectanglePro(dest, origin, rotation, drawTint);
        }
    }

    void setter(float newX, float newY) {
        x = newX;
        y = newY;
    }
};
