#include "Chomper.h"

Chomper::Chomper(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 150, "Chomper") {
    // Load reanim for Chomper
    getResources(res.GetAssetPath("assets/reanim/Chomper.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Chomper::~Chomper() {
}

void Chomper::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);
}

void Chomper::draw() {
    m_anim.Draw(m_x, m_y, 1.0f); 
}
