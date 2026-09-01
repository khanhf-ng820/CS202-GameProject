#include "PotatoMine.h"
#include "AudioManager.h"

PotatoMine::PotatoMine(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 25, "PotatoMine") {
    getResources(res.GetAssetPath("assets/reanim/PotatoMine.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle"); // Unarmed/buried state
}

PotatoMine::~PotatoMine() {
}

void PotatoMine::triggerExplode() {
    if (m_state == PotatoMineState::ARMED) {
        m_state = PotatoMineState::EXPLODING;
        m_anim.SetBaseAnimation("anim_mashed");
        m_anim.SetAnimation("anim_mashed");
        m_explodeTimer = 0.0f;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/potato_mine.ogg"));

        // Spawn flying potato chunk particles
        m_particles.clear();
        float cx = (float)m_x + 40.0f;
        float cy = (float)m_y + 40.0f;
        for (int i = 0; i < 16; ++i) {
            PotatoChunkParticle p;
            p.x = cx + (float)GetRandomValue(-10, 10);
            p.y = cy + (float)GetRandomValue(-10, 10);
            p.vx = (float)GetRandomValue(-160, 160);
            p.vy = (float)GetRandomValue(-280, -80); // Upward blast
            p.rotation = (float)GetRandomValue(0, 360);
            p.rotSpeed = (float)GetRandomValue(-450, 450);
            p.scale = (float)GetRandomValue(9, 13) / 10.0f;
            p.frameIndex = GetRandomValue(0, 5);
            p.alpha = 1.0f;
            m_particles.push_back(p);
        }
    }
}

void PotatoMine::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    switch (m_state) {
        case PotatoMineState::UNARMED:
            m_armTimer += deltaTime;
            if (m_armTimer >= ARM_DELAY) {
                m_state = PotatoMineState::RISING;
                m_anim.SetBaseAnimation("anim_rise");
                m_anim.SetAnimation("anim_rise");
            }
            break;

        case PotatoMineState::RISING:
            if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
                m_state = PotatoMineState::ARMED;
                m_anim.SetBaseAnimation("anim_armed");
                m_anim.SetAnimation("anim_armed");
            }
            break;

        case PotatoMineState::ARMED:
            // Waiting for zombie to trigger — handled by Level1::updateCollisions
            break;

        case PotatoMineState::EXPLODING:
            m_explodeTimer += deltaTime;

            // Update potato particle physics
            for (auto& p : m_particles) {
                p.vy += 450.0f * deltaTime; // Gravity
                p.x += p.vx * deltaTime;
                p.y += p.vy * deltaTime;
                p.rotation += p.rotSpeed * deltaTime;
                if (m_explodeTimer > 0.5f) {
                    p.alpha = std::max(0.0f, (1.0f - m_explodeTimer) / 0.5f);
                }
            }

            if (m_explodeTimer >= 1.0f) {
                m_state = PotatoMineState::DONE;
                takeDamage(m_hp); // Mark as dead after explosion sequence
            }
            break;

        case PotatoMineState::DONE:
            break;
    }
}

void PotatoMine::draw() {
    if (m_state == PotatoMineState::EXPLODING) {
        float cx = (float)m_x + 40.0f;
        float cy = (float)m_y + 40.0f;

        // 1. Radial Flash Burst
        if (m_explodeTimer <= 0.25f) {
            Texture2D flashTex = res.GetTexture("PotatoMineFlash");
            if (flashTex.id != 0) {
                float flashProgress = m_explodeTimer / 0.25f;
                float flashScale = 0.5f + 1.0f * (1.0f - powf(1.0f - flashProgress, 2.0f));
                unsigned char flashAlpha = (unsigned char)(200 * (1.0f - flashProgress));
                Color flashTint = { 255, 255, 200, flashAlpha };
                float fw = flashTex.width * flashScale;
                float fh = flashTex.height * flashScale;
                DrawTexturePro(flashTex,
                    { 0, 0, (float)flashTex.width, (float)flashTex.height },
                    { cx, cy, fw, fh },
                    { fw / 2.0f, fh / 2.0f },
                    0.0f,
                    flashTint);
            }
        }

        // 2. Ground Mashed Potato Mound
        Texture2D mashedTex = res.GetTexture("PotatoMine_mashed");
        if (mashedTex.id != 0) {
            float progress = m_explodeTimer / 1.0f;
            if (progress > 1.0f) progress = 1.0f;
            unsigned char alpha = 255;
            if (progress > 0.6f) {
                alpha = (unsigned char)(255 * (1.0f - progress) / 0.4f);
            }
            Color tint = { 255, 255, 255, alpha };
            DrawTextureEx(mashedTex, Vector2{ (float)m_x + 6.0f, (float)m_y + 24.0f }, 0.0f, 0.8f, tint);
        }

        // 3. Flying Potato Particles
        Texture2D partTex = res.GetTexture("PotatoMine_particles");
        if (partTex.id != 0) {
            for (const auto& p : m_particles) {
                if (p.alpha <= 0.01f) continue;
                Rectangle srcRec = { p.frameIndex * 23.0f, 0.0f, 23.0f, 26.0f };
                float destW = 23.0f * p.scale;
                float destH = 26.0f * p.scale;
                Rectangle destRec = { p.x, p.y, destW, destH };
                Vector2 origin = { destW / 2.0f, destH / 2.0f };
                unsigned char pAlpha = (unsigned char)(255 * p.alpha);
                DrawTexturePro(partTex, srcRec, destRec, origin, p.rotation, Color{ 255, 255, 255, pAlpha });
            }
        }

        // 4. "SPUDOW!!" Comic Text
        if (m_explodeTimer <= 0.85f) {
            Texture2D spudowTex = res.GetTexture("ExplosionSpudow");
            if (spudowTex.id != 0) {
                float spudowProgress = m_explodeTimer / 0.85f;
                float tPop = std::min(1.0f, m_explodeTimer / 0.15f);
                float spudowScale = 0.5f + 0.65f * (1.0f - powf(1.0f - tPop, 3.0f));
                unsigned char spudowAlpha = 255;
                if (spudowProgress > 0.4f) {
                    spudowAlpha = (unsigned char)(255 * (1.0f - spudowProgress) / 0.6f);
                }
                float shakeX = (m_explodeTimer < 0.25f) ? (float)GetRandomValue(-2, 2) : 0.0f;
                float shakeY = (m_explodeTimer < 0.25f) ? (float)GetRandomValue(-2, 2) : 0.0f;
                float sw = spudowTex.width * spudowScale;
                float sh = spudowTex.height * spudowScale;
                DrawTexturePro(spudowTex,
                    { 0, 0, (float)spudowTex.width, (float)spudowTex.height },
                    { cx + shakeX, cy - 25.0f + shakeY, sw, sh },
                    { sw / 2.0f, sh / 2.0f },
                    0.0f,
                    Color{ 255, 255, 255, spudowAlpha });
            }
        }
    } else if (m_state != PotatoMineState::DONE) {
        m_anim.Draw(m_x, m_y, 0.8f);
    }
}

bool PotatoMine::isDead() const {
    return m_state == PotatoMineState::DONE;
}
