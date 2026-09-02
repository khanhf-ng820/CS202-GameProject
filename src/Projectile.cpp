#include "Projectile.h"
#include "AudioManager.h"


void Projectile::onHit() {
    if (m_isImpacting || !m_active) return;
    m_isImpacting = true;
    m_speed = 0.0f;
    efftrailing.setActive(false);

    if (m_isLobbed) {
        m_y = m_startY + 60.0f;
    }

    if (m_isMelon) {
        if (m_res) {
            std::string soundPath = m_res->GetAssetPath("assets/sounds/melonimpact.ogg");
            if (GetRandomValue(0, 1) == 1) {
                soundPath = m_res->GetAssetPath("assets/sounds/melonimpact2.ogg");
            }
            AudioManager::GetInstance().PlaySoundEffect(soundPath);
        }

        Texture2D melonParticleTex = m_res ? m_res->GetTexture("MELONPULT_PARTICLES") : Texture2D{0};
        if (melonParticleTex.id == 0 && m_res) melonParticleTex = m_res->GetTexture("Melonpult_particles");

        int particleCount = 15;
        for (int i = 0; i < particleCount; ++i) {
            ParticleEffect p;
            p.x = m_x + (float)GetRandomValue(-10, 20);
            p.y = m_y + (float)GetRandomValue(-10, 10);

            float angle = (float)GetRandomValue(0, 360) * 3.14159265f / 180.0f;
            float speed = (float)GetRandomValue(100, 200);
            p.vx = cosf(angle) * speed;
            p.vy = sinf(angle) * speed - (float)GetRandomValue(40, 120);

            p.gravity = 350.0f;
            p.rotation = (float)GetRandomValue(0, 360);
            p.vr = (float)GetRandomValue(-1120, 1120);
            p.scale = (float)GetRandomValue(9, 14) / 10.0f;
            p.alpha = 1.0f;
            p.fadeRate = 2.0f;
            p.isPhysicsParticle = true;
            p.active = true;

            p.totalFrames = 9;
            p.currentFrame = GetRandomValue(0, 8);
            if (melonParticleTex.id != 0) {
                p.texture = melonParticleTex;
            }

            m_sparks.push_back(p);
        }
    } else if (m_isFire) {
        Texture2D sparkTex = m_res ? m_res->GetTexture("FirePea_spark") : Texture2D{0};
        if (sparkTex.id == 0 && m_res) sparkTex = m_res->GetTexture("FIREPEA_SPARK");

        int sparkCount = GetRandomValue(10, 14);
        for (int i = 0; i < sparkCount; ++i) {
            ParticleEffect p;
            p.x = m_x + (float)GetRandomValue(-5, 5);
            p.y = m_y + (float)GetRandomValue(-5, 5);
            p.vx = (float)GetRandomValue(-160, 160);
            p.vy = (float)GetRandomValue(-200, 30);  // Fly outward & pop up
            p.gravity = 450.0f;                      // Pull sparks back down
            p.rotation = (float)GetRandomValue(0, 360);
            p.vr = (float)GetRandomValue(-600, 600);
            p.scale = (float)GetRandomValue(9, 18) / 10.0f;
            p.alpha = 1.0f;
            p.fadeRate = 3.2f;                       // Fades out in ~0.3s
            p.isPhysicsParticle = true;
            p.active = true;
            if (sparkTex.id != 0) {
                p.texture = sparkTex;
            }

            int c = GetRandomValue(0, 2);
            if (c == 0) p.tint = Color{ 255, 230, 80, 255 };      // Vivid Yellow / Gold
            else if (c == 1) p.tint = Color{ 255, 140, 20, 255 }; // Fiery Orange
            else p.tint = Color{ 255, 50, 10, 255 };              // Fiery Red

            m_sparks.push_back(p);
        }
    } else {
        // Standard pea or snow pea splat effect
        ParticleEffect eff;
        if (m_isSnow) {
            eff.texture = m_res ? m_res->GetTexture("SNOWPEA_SPLATS") : Texture2D{0};
            if (eff.texture.id == 0 && m_res) eff.texture = m_res->GetTexture("SnowPea_splats");
            eff.totalFrames = 4;
        } else {
            eff.texture = m_res ? m_res->GetTexture("PEA_SPLATS") : Texture2D{0};
            if (eff.texture.id == 0 && m_res) eff.texture = m_res->GetTexture("pea_splats");
            eff.totalFrames = 4;
        }

        if (eff.texture.id != 0) {
            eff.x = m_x;
            eff.y = m_y;
            eff.currentFrame = 0;
            eff.frameDuration = 0.05f;
            eff.timer = 0.0f;
            eff.scale = 1.2f;
            eff.active = true;
            m_sparks.push_back(eff);
        } else {
            m_active = false;
        }
    }
}

void Projectile::update(float dt) {
    if (!m_isImpacting) {
        if (m_strategy) {
            m_strategy->updatePosition(m_x, m_y, m_startX, m_startY, m_speed, m_range, m_maxHeight, m_progress, dt);
            if (m_isLobbed && m_progress >= 1.0f) {
                onHit();
            }
        } else {
            m_x += m_speed * dt;
        }
        if (m_x > 1300) {
            m_active = false;
        }
        if (m_isFire && m_hasReanim) {
            m_fireAnim.Update(dt);
        }
        efftrailing.timing(dt);
        efftrailing.setter(m_x, m_y + (m_tex.height * m_scale) / 2.0f);
    } else {
        bool anyActive = false;
        for (auto& eff : m_sparks) {
            if (eff.active) {
                eff.timing(dt);
                if (eff.active) anyActive = true;
            }
        }
        if (!anyActive) {
            m_active = false;
        }
    }
}

void Projectile::draw() const {
    if (m_active) {
        if (!m_isImpacting) {
            if (m_isFire && m_hasReanim) {
                m_fireAnim.Draw(m_x + 3.0f, m_y - 22.0f, m_scale * 1.0f);
            } else if (m_tex.id != 0) {
                DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, m_scale, WHITE);
            }
            efftrailing.draw();
        } else {
            for (const auto& eff : m_sparks) {
                eff.draw();
            }
        }
    }
}
