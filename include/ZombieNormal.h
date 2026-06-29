#pragma once
#include "Zombie.h"

#include <vector>

struct FallingPart {
    Texture2D texture;
    float x, y;
    float vx, vy;
    float rotation;
    float rotSpeed;
    float timer;
    bool active;
};

class ZombieNormal : public Zombie {
public:
    ZombieNormal(Resources& res, float x, float y);
    ~ZombieNormal() override;

    void update(float deltaTime) override;
    void draw() override;

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
};
