#pragma once
#include "Level4.h"

class Level5 : public Level4 {
public:
    Level5(Resources& res, RenderTexture2D targetScreen);
    virtual ~Level5() = default;

protected:
    std::vector<std::string> getUniqueLevelZombieTypes() const override;
    void initGraves() override;
    void spawnNextWave() override;
};
