#pragma once
#include "Level4.h"

class Level6 : public Level4 {
public:
    Level6(Resources& res, RenderTexture2D targetScreen);
    virtual ~Level6() = default;

protected:
    std::vector<std::string> getUniqueLevelZombieTypes() const override;
    void initGraves() override;
    void spawnNextWave() override;
};
