#include "Zombie.h"

Zombie::Zombie(Resources& r, float x, float y, int hp, float speed, int damage, std::string name)
    : res(r), m_x(x), m_y(y), m_hp(hp), m_maxHp(hp), m_speed(speed), m_damage(damage), m_name(name), m_isEating(false) {
}

void Zombie::getResources(std::string dir) {
    m_anim.SetResources(res.LoadReanim(dir), res);
}

void Zombie::takeExplosiveDamage(float damage) {
    if (m_hp <= 0) return;
    m_hp -= damage;
    if (m_hp <= 0) {
        m_isCharred = true;
        m_charredAnim.SetResources(res.LoadReanim(res.GetAssetPath("assets/reanim/Zombie_charred.reanim")), res);
        m_charredAnim.SetAnimation("anim_crumble");
    }
}

void Zombie::takeSquashDamage(float damage) {
    if (m_hp <= 0) return;
    m_hp -= damage;
    if (m_hp <= 0) {
        m_isSquashed = true;
        m_squashTimer = 0.0f;
    }
}
