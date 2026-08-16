#include "BowlingNut.h"

BowlingNut::BowlingNut(float x, float y, float vx, float vy, float rotSpeed)
    : m_x(x), m_y(y), m_vx(vx), m_vy(vy), m_rotationAngle(0.0f), m_rotationSpeed(rotSpeed) {}

std::unique_ptr<BowlingNut> BowlingNut::Create(const std::string& plantType, float x, float y) {
    if (plantType == "GiantWallnut") {
        return std::make_unique<GiantBowlingNut>(x, y);
    } else if (plantType == "ExplodeNut") {
        return std::make_unique<ExplodeBowlingNut>(x, y);
    }
    return std::make_unique<NormalBowlingNut>(x, y);
}

// ==========================================
// NormalBowlingNut
// ==========================================
NormalBowlingNut::NormalBowlingNut(float x, float y)
    : BowlingNut(x, y, 300.0f, 0.0f, 360.0f) {}

void NormalBowlingNut::update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) {
    m_x += m_vx * dt;
    m_y += m_vy * dt;
    m_rotationAngle += m_rotationSpeed * dt;
    if (m_rotationAngle >= 360.0f) {
        m_rotationAngle -= 360.0f;
    }

    if (m_hitCooldown > 0.0f) {
        m_hitCooldown -= dt;
    }

    // Top / Bottom lawn boundary bounce (top edge of row 0: y = 80.0f, bottom edge of row 4: y = 580.0f)
    if (m_y <= 80.0f && m_vy < 0.0f) {
        m_y = 80.0f;
        m_vy = -m_vy;
    } else if (m_y >= 580.0f && m_vy > 0.0f) {
        m_y = 580.0f;
        m_vy = -m_vy;
    }

    for (auto& z : zombies) {
        if (!z->isDead() && m_hitCooldown <= 0.0f && z.get() != m_lastHitZombie) {
            float zCx = z->getX() + 40.0f;
            float zCy = z->getY() + 80.0f;
            float dx = m_x - zCx;
            float dy = m_y - zCy;

            if (dx <= 10.0f) {
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist <= 42.0f) {
                    // Deal zombie-type specific damage
                    int damage = 200;
                    std::string zName = z->getName();
                    if (zName == "BucketheadZombie") {
                        if (z->getHp() > 600) {
                            damage = 300; // Hit 1: 810 -> 510 (heavily dented bucket)
                        } else if (z->getHp() > 200) {
                            damage = 310; // Hit 2: 510 -> 200 (loses bucket)
                        } else {
                            damage = 200; // Hit 3: 200 -> 0 (dies)
                        }
                    } else if (zName == "ConeheadZombie") {
                        if (z->getHp() > 200) {
                            damage = 340; // Hit 1: 540 -> 200 (loses cone)
                        } else {
                            damage = 200; // Hit 2: 200 -> 0 (dies)
                        }
                    } else if (zName == "PoleVaultingZombie") {
                        damage = 250; // 2 hits to defeat (500 HP)
                    } else {
                        damage = 200; // NormalZombie, FlagZombie: 1 hit death
                    }

                    z->takeDamage(damage);

                    if (m_vy == 0.0f) {
                        float dir = (GetRandomValue(0, 1) == 0) ? -180.0f : 180.0f;
                        m_vy = dir;
                    } else {
                        m_vy = -m_vy;
                    }
                    m_lastHitZombie = z.get();
                    m_hitCooldown = 0.25f;

                    bool foundDebug = false;
                    for (auto& item : hitDebugTimers) {
                        if (item.first == z.get()) {
                            item.second = 0.6f;
                            foundDebug = true;
                            break;
                        }
                    }
                    if (!foundDebug) {
                        hitDebugTimers.push_back({ z.get(), 0.6f });
                    }

                    AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/bowling.ogg"));
                    break;
                }
            }
        }
    }
}

void NormalBowlingNut::draw(Resources& res, bool showDebug) const {
    Texture2D nutBodyTex = res.GetTexture("WALLNUT_BODY");
    if (nutBodyTex.id == 0) nutBodyTex = res.GetTexture("Wallnut_body");
    if (nutBodyTex.id == 0) {
        std::string bodyPath = res.GetAssetPath("assets/reanim/Wallnut_body.png");
        res.LoadFile(bodyPath);
        nutBodyTex = res.GetTexture("WALLNUT_BODY");
    }

    float renderW = 60.0f;
    float renderH = 65.0f;
    Rectangle destRec = { m_x, m_y, renderW, renderH };
    Vector2 origin = { renderW / 2.0f, renderH / 2.0f };

    if (nutBodyTex.id != 0) {
        DrawTexturePro(
            nutBodyTex,
            { 0.0f, 0.0f, (float)nutBodyTex.width, (float)nutBodyTex.height },
            destRec,
            origin,
            m_rotationAngle,
            WHITE
        );
    } else {
        DrawCircle((int)m_x, (int)m_y, 30.0f, BROWN);
    }

    if (showDebug) {
        DrawRectangleLinesEx({ m_x - 30.0f, m_y - 32.5f, 60.0f, 65.0f }, 2.0f, RED);
        DrawCircle((int)m_x, (int)m_y, 4.0f, RED);
    }
}

// ==========================================
// GiantBowlingNut
// ==========================================
GiantBowlingNut::GiantBowlingNut(float x, float y)
    : BowlingNut(x, y, 300.0f, 0.0f, 360.0f) {}

void GiantBowlingNut::update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) {
    m_x += m_vx * dt;
    m_y += m_vy * dt;
    m_rotationAngle += m_rotationSpeed * dt;
    if (m_rotationAngle >= 360.0f) {
        m_rotationAngle -= 360.0f;
    }

    m_vx = 300.0f;
    m_vy = 0.0f;

    for (auto& z : zombies) {
        if (!z->isDead()) {
            float zCx = z->getX() + 40.0f;
            float zCy = z->getY() + 80.0f;

            if (fabsf(m_y - zCy) < 10.0f) {
                float dx = m_x - zCx;
                float dy = m_y - zCy;

                if (dx <= 10.0f) {
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= 84.0f) {
                        if (std::find(m_hitZombies.begin(), m_hitZombies.end(), z.get()) == m_hitZombies.end()) {
                            m_hitZombies.push_back(z.get());

                            // Instantly kill the zombie (triggers armor detachment and death animations)
                            z->takeDamage(1000);

                            bool foundDebug = false;
                            for (auto& item : hitDebugTimers) {
                                if (item.first == z.get()) {
                                    item.second = 0.6f;
                                    foundDebug = true;
                                    break;
                                }
                            }
                            if (!foundDebug) {
                                hitDebugTimers.push_back({ z.get(), 0.6f });
                            }

                            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/bowlingimpact2.ogg"));
                        }
                    }
                }
            }
        }
    }
}

void GiantBowlingNut::draw(Resources& res, bool showDebug) const {
    Texture2D nutBodyTex = res.GetTexture("WALLNUT_BODY");
    if (nutBodyTex.id == 0) nutBodyTex = res.GetTexture("Wallnut_body");
    if (nutBodyTex.id == 0) {
        std::string bodyPath = res.GetAssetPath("assets/reanim/Wallnut_body.png");
        res.LoadFile(bodyPath);
        nutBodyTex = res.GetTexture("WALLNUT_BODY");
    }

    float renderW = 120.0f;
    float renderH = 130.0f;
    Rectangle destRec = { m_x, m_y - 20.0f, renderW, renderH };
    Vector2 origin = { renderW / 2.0f, renderH / 2.0f };

    if (nutBodyTex.id != 0) {
        DrawTexturePro(
            nutBodyTex,
            { 0.0f, 0.0f, (float)nutBodyTex.width, (float)nutBodyTex.height },
            destRec,
            origin,
            m_rotationAngle,
            WHITE
        );
    } else {
        DrawCircle((int)m_x, (int)(m_y - 20.0f), 60.0f, BROWN);
    }

    if (showDebug) {
        DrawRectangleLinesEx({ m_x - 60.0f, m_y - 20.0f - 65.0f, 120.0f, 130.0f }, 2.0f, RED);
        DrawCircle((int)m_x, (int)m_y, 4.0f, RED);
    }
}

// ==========================================
// ExplodeBowlingNut
// ==========================================
ExplodeBowlingNut::ExplodeBowlingNut(float x, float y)
    : BowlingNut(x, y, 300.0f, 0.0f, 360.0f) {}

void ExplodeBowlingNut::update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) {
    if (m_isExplodingEffect) {
        m_vx = 0.0f;
        m_vy = 0.0f;
        m_rotationSpeed = 0.0f;
        m_explosionTimer += dt;
        if (m_explosionTimer >= 0.5f) {
            m_hasExploded = true;
        }
        return;
    }

    m_x += m_vx * dt;
    m_y += m_vy * dt;
    m_rotationAngle += m_rotationSpeed * dt;
    if (m_rotationAngle >= 360.0f) {
        m_rotationAngle -= 360.0f;
    }

    m_vx = 300.0f;
    m_vy = 0.0f;

    for (auto& z : zombies) {
        if (!z->isDead()) {
            float zCx = z->getX() + 40.0f;
            float zCy = z->getY() + 80.0f;

            if (fabsf(m_y - zCy) < 10.0f) {
                float dx = m_x - zCx;
                float dy = m_y - zCy;

                if (dx <= 10.0f) {
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= 42.0f) {
                        m_isExplodingEffect = true;
                        m_explosionTimer = 0.0f;
                        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/cherrybomb.ogg"));

                        // Explode in 150px circle radius, instantly incinerating all zombies in range
                        for (auto& targetZ : zombies) {
                            if (!targetZ->isDead()) {
                                float targetZcx = targetZ->getX() + 40.0f;
                                float targetZcy = targetZ->getY() + 80.0f;
                                float exDx = targetZcx - m_x;
                                float exDy = targetZcy - m_y;
                                if (exDx * exDx + exDy * exDy <= 150.0f * 150.0f) {
                                    targetZ->takeExplosiveDamage(1800.0f);

                                    bool foundDebug = false;
                                    for (auto& item : hitDebugTimers) {
                                        if (item.first == targetZ.get()) {
                                            item.second = 0.6f;
                                            foundDebug = true;
                                            break;
                                        }
                                    }
                                    if (!foundDebug) {
                                        hitDebugTimers.push_back({ targetZ.get(), 0.6f });
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void ExplodeBowlingNut::draw(Resources& res, bool showDebug) const {
    if (m_isExplodingEffect) {
        Texture2D powTex = res.GetTexture("Pow");
        if (powTex.id == 0) powTex = res.GetTexture("POW");
        if (powTex.id == 0) {
            res.LoadFile(res.GetAssetPath("assets/reanim/Pow.png"));
            powTex = res.GetTexture("Pow");
            if (powTex.id == 0) powTex = res.GetTexture("POW");
        }

        Texture2D powieTex = res.GetTexture("ExplosionPowie");
        if (powieTex.id == 0) powieTex = res.GetTexture("EXPLOSIONPOWIE");
        if (powieTex.id == 0) {
            res.LoadFile(res.GetAssetPath("assets/reanim/ExplosionPowie.png"));
            powieTex = res.GetTexture("ExplosionPowie");
            if (powieTex.id == 0) powieTex = res.GetTexture("EXPLOSIONPOWIE");
        }

        float progress = m_explosionTimer / 0.5f; // 0.0 -> 1.0
        if (progress > 1.0f) progress = 1.0f;

        // Scale out quickly (ease-out cubic)
        float scale = 0.5f + 2.5f * (1.0f - powf(1.0f - progress, 3.0f));

        // Fade out
        unsigned char alpha = 255;
        if (progress > 0.5f) {
            alpha = (unsigned char)(255 * (1.0f - progress) / 0.5f);
        }
        Color tint = { 255, 255, 255, alpha };

        if (powieTex.id != 0) {
            DrawTexturePro(
                powieTex,
                { 0, 0, (float)powieTex.width, (float)powieTex.height },
                { m_x, m_y, powieTex.width * scale * 1.5f, powieTex.height * scale * 1.5f },
                { powieTex.width * scale * 1.5f / 2.0f, powieTex.height * scale * 1.5f / 2.0f },
                progress * 90.0f,
                tint
            );
        }

        if (powTex.id != 0) {
            DrawTexturePro(
                powTex,
                { 0, 0, (float)powTex.width, (float)powTex.height },
                { m_x, m_y, powTex.width * scale, powTex.height * scale },
                { powTex.width * scale / 2.0f, powTex.height * scale / 2.0f },
                0.0f,
                tint
            );
        }

        return;
    }

    Texture2D nutBodyTex = res.GetTexture("WALLNUT_BODY");
    if (nutBodyTex.id == 0) nutBodyTex = res.GetTexture("Wallnut_body");
    if (nutBodyTex.id == 0) {
        std::string bodyPath = res.GetAssetPath("assets/reanim/Wallnut_body.png");
        res.LoadFile(bodyPath);
        nutBodyTex = res.GetTexture("WALLNUT_BODY");
    }

    float renderW = 60.0f;
    float renderH = 65.0f;
    Rectangle destRec = { m_x, m_y, renderW, renderH };
    Vector2 origin = { renderW / 2.0f, renderH / 2.0f };

    if (nutBodyTex.id != 0) {
        DrawTexturePro(
            nutBodyTex,
            { 0.0f, 0.0f, (float)nutBodyTex.width, (float)nutBodyTex.height },
            destRec,
            origin,
            m_rotationAngle,
            RED
        );
    } else {
        DrawCircle((int)m_x, (int)m_y, 30.0f, RED);
    }

    if (showDebug) {
        DrawRectangleLinesEx({ m_x - 30.0f, m_y - 32.5f, 60.0f, 65.0f }, 2.0f, RED);
        DrawCircle((int)m_x, (int)m_y, 4.0f, RED);
    }
}
