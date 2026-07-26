#include "Torchwood.h"

Torchwood::Torchwood(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 175, "Torchwood") {
    getResources(res.GetAssetPath("assets/reanim/Torchwood.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Torchwood::~Torchwood() {
}

void Torchwood::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);
}

void Torchwood::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
