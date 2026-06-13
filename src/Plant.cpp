#include "plant.h"

Plant::Plant(Resources& r, int x, int y, int hp, int sunCost, std::string name)
    : res(r), m_x(x), m_y(y), m_hp(hp), m_maxHp(hp), m_sunCost(sunCost), m_name(name) {
    m_cooldownTime = 0.0f;
}

void Plant::getResources(std::string dir) {
    m_anim.SetResources(res.LoadReanim(dir), res);
}
