#include "SunFlower.h"

SunFlower::SunFlower(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 50, "SunFlower") {
    getResources(res.GetAssetPath("assets/reanim/SunFlower.reanim"));
    m_anim.SetAnimation("anim_idle");
}

SunFlower::~SunFlower() {
}

void SunFlower::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    m_sunProduceTimer += deltaTime;
    if (m_sunProduceTimer >= m_sunProduceInterval) {
        m_sunProduceTimer = 0.0f;

        // Get the Sun texture (Sun3.png or Sun1.png)
        Texture2D sunTex = res.GetTexture("Sun3");
        if (sunTex.id == 0) {
            sunTex = res.GetTexture("Sun1");
        }

        // Spawn a SunItem at Sunflower's position
        outSuns.push_back(SunItem(m_x + 20, m_y - 20, sunTex));
    }
}

void SunFlower::draw() {
    // Beautiful golden glow right before producing a sun
    if (m_sunProduceInterval - m_sunProduceTimer <= 0.6f) {
        float glowIntensity = (0.6f - (m_sunProduceInterval - m_sunProduceTimer)) / 0.6f;
        unsigned char r = (unsigned char)(255);
        unsigned char g = (unsigned char)(255);
        unsigned char b = (unsigned char)(150 + 105 * (1.0f - glowIntensity));
        m_anim.Draw(m_x, m_y, 1.0f, Color{r, g, b, 255});
    } else {
        m_anim.Draw(m_x, m_y, 1.0f);
    }
}
