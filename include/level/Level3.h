#pragma once
#include "Level1.h"

class Level3 : public Level1 {
public:
    Level3(Resources& res, RenderTexture2D targetScreen);
    virtual ~Level3() = default;

protected:
    std::vector<std::string> getUniqueLevelZombieTypes() const override;
    void spawnNextWave() override;
};
