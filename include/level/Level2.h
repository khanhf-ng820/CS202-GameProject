#pragma once
#include "Level1.h"

class Level2 : public Level1 {
public:
    Level2(Resources& res, RenderTexture2D targetScreen);
    virtual ~Level2() = default;

protected:
    std::vector<std::string> getUniqueLevelZombieTypes() const override;
    void spawnNextWave() override;
};
