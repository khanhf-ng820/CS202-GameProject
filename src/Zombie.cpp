#include "Zombie.h"

Zombie::Zombie(Resources& r, float x, float y, int hp, float speed, int damage, std::string name)
    : res(r), m_x(x), m_y(y), m_hp(hp), m_maxHp(hp), m_speed(speed), m_damage(damage), m_name(name), m_isEating(false) {
}

void Zombie::getResources(std::string dir) {
    m_anim.SetResources(res.LoadReanim(dir), res);
}
