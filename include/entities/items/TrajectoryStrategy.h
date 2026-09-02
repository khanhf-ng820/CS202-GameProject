#pragma once
#include <memory>
#include <cmath>

// Strategy interface for projectile trajectory calculation
class ITrajectoryStrategy {
public:
    virtual ~ITrajectoryStrategy() = default;
    virtual void updatePosition(float& x, float& y, float startX, float startY, float speed, float range, float maxHeight, float& progress, float dt) = 0;
};

// Linear motion trajectory strategy (Peas, SnowPeas, FirePeas)
class StraightTrajectoryStrategy : public ITrajectoryStrategy {
public:
    void updatePosition(float& x, float& y, float startX, float startY, float speed, float range, float maxHeight, float& progress, float dt) override {
        x += speed * dt;
    }
};

// Parabolic arc trajectory strategy for lobbed projectiles (Cabbage, Corn, Melon)
class LobbedTrajectoryStrategy : public ITrajectoryStrategy {
public:
    void updatePosition(float& x, float& y, float startX, float startY, float speed, float range, float maxHeight, float& progress, float dt) override {
        if (range <= 0.0f) range = 400.0f;
        
        progress += (speed / range) * dt;
        if (progress > 1.0f) progress = 1.0f;

        x = startX + progress * range;
        float heightOffset = 4.0f * maxHeight * progress * (1.0f - progress);
        y = startY - heightOffset;
    }
};
