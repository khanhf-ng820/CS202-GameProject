#include "Plantern.h"

Plantern::Plantern(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 25, "Plantern") {
    getResources(res.GetAssetPath("assets/reanim/Plantern.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Plantern::~Plantern() {
}

void Plantern::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);
}

void Plantern::draw() {
    // Soft ambient illumination halo
    DrawCircleGradient(m_x + 38, m_y + 45, 75.0f, ColorAlpha(YELLOW, 0.20f), ColorAlpha(YELLOW, 0.0f));
    DrawCircleGradient(m_x + 38, m_y + 45, 45.0f, ColorAlpha(WHITE, 0.25f), ColorAlpha(GOLD, 0.0f));
    m_anim.Draw(m_x, m_y, 1.0f);
}
