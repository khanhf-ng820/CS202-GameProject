#pragma once
#include "raylib.h"
#include "resources.h"
#include "Plant.h"
#include "Zombie.h"
#include "Projectile.h"
#include "SunItem.h"
#include <vector>
#include <memory>

class Testing {
public:
    Testing(Resources& res, RenderTexture2D targetScreen);
    void run();

private:
    Resources& res;
    RenderTexture2D targetScreen;
    
    int currentPlantType;
    std::unique_ptr<Plant> currentPlant;
    std::vector<Projectile> projectiles;
    std::vector<SunItem> suns;
    std::unique_ptr<Zombie> currentZombie;
    int currentZombieType;
};
