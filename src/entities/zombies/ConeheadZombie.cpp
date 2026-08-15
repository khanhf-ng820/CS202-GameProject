#include "ConeheadZombie.h"
#include "rlgl.h"
#include <algorithm>

ConeheadZombie::ConeheadZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 560, 8.0f, 100, "ConeheadZombie") {
    
    getResources(res.GetAssetPath("assets/reanim/Zombie.reanim"));
    m_anim.SetBaseAnimation("anim_walk");
    m_anim.SetAnimation("anim_walk");

    m_anim.SetTrackVisible("anim_bucket", false);
    m_anim.SetTrackVisible("anim_cone", true);
    m_anim.SetTrackVisible("anim_screendoor", false);
    m_anim.SetTrackVisible("Zombie_duckytube", false);
    m_anim.SetTrackVisible("Zombie_mustache", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
    m_anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_flaghand", false);
}

ConeheadZombie::~ConeheadZombie() {}

void ConeheadZombie::takeDamage(float damage) {
    if (m_isCharred || m_isSquashed || m_isDevoured) return;

    if (!m_hasLostCone && (m_hp - damage <= 200)) {
        m_hasLostCone = true;
        m_anim.SetTrackVisible("anim_cone", false);

        Resources& res = Resources::GetInstance();
        FallingPart cone;
        cone.texture = res.GetTexture("ZOMBIE_CONE3");
        cone.x = m_x + 30.0f;
        cone.y = m_y + 10.0f;
        cone.vx = (float)GetRandomValue(-30, 30);
        cone.vy = (float)GetRandomValue(-150, -50);
        cone.rotation = 0;
        cone.rotSpeed = (float)GetRandomValue(-150, 150);
        cone.timer = 1.0f;
        cone.active = true;
        m_fallingParts.push_back(cone);
    }

    Zombie::takeDamage(damage);
    if (m_hp <= 0) {
        if (m_anim.GetCurrentAnimName() != "anim_death" && m_anim.GetCurrentAnimName() != "anim_death2") {
            m_anim.SetAnimation("anim_death");
        }
    } else if (m_hp <= 100 && !m_hasLostArm) {
        m_hasLostArm = true;
        Resources& res = Resources::GetInstance();
        FallingPart arm;
        arm.texture = res.GetTexture("ZOMBIE_OUTERARM_LOWER");
        arm.x = m_x + 45.0f; 
        arm.y = m_y + 110.0f;
        arm.vx = (float)GetRandomValue(-40, 20);
        arm.vy = (float)GetRandomValue(-120, -40);
        arm.rotation = 0;
        arm.rotSpeed = (float)GetRandomValue(-150, 150);
        arm.timer = 1.0f;
        arm.active = true;
        m_fallingParts.push_back(arm);
    }
}

void ConeheadZombie::update(float deltaTime) {
    if (m_isDevoured) return;

    if (m_isSquashed) {
        m_squashTimer += deltaTime;
        return;
    }

    if (m_isCharred) {
        m_charredAnim.Update(deltaTime);
        m_charredTimer += deltaTime;
        if (m_charredAnim.GetCurrentFrame() >= m_charredAnim.GetEndFrame() - 1) {
            m_charredAnim.SetPaused(true);
        }
        return;
    }

    if (!m_hasLostCone) {
        if (m_hp <= 310) {
            m_anim.OverrideTrackImage("anim_cone", "Zombie_cone3");
        } else if (m_hp <= 430) {
            m_anim.OverrideTrackImage("anim_cone", "Zombie_cone2");
        } else {
            m_anim.OverrideTrackImage("anim_cone", "Zombie_cone1");
        }
    }

    m_anim.Update(deltaTime);

    if (m_hp <= 0) {
        m_deathTimer += deltaTime;
        if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
            m_anim.SetPaused(true);
        }
    }

    std::string currentAnim = m_anim.GetCurrentAnimName();

    if (currentAnim == "anim_death" || currentAnim == "anim_death2" || currentAnim == "anim_waterdeath") {
        m_anim.SetTrackVisible("anim_head1", false);
        m_anim.SetTrackVisible("anim_hair", false);
        m_anim.SetTrackVisible("anim_head2", false);
        m_anim.SetTrackVisible("anim_tongue", false);
        m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
        m_anim.SetTrackVisible("anim_cone", false);

        if (!m_hasSpawnedDeathParts) {
            m_hasSpawnedDeathParts = true;
            Resources& res = Resources::GetInstance();

            FallingPart head;
            head.texture = res.GetTexture("ZOMBIE_HEAD");
            head.x = m_x + 60.0f; 
            head.y = m_y + 10.0f;
            head.vx = (float)GetRandomValue(-30, 30);
            head.vy = (float)GetRandomValue(-150, -50);
            head.rotation = 0;
            head.rotSpeed = (float)GetRandomValue(-100, 100);
            head.timer = 1.0f;
            head.active = true;
            m_fallingParts.push_back(head);

            FallingPart jaw;
            jaw.texture = res.GetTexture("ZOMBIE_JAW");
            jaw.x = m_x + 60.0f; 
            jaw.y = m_y + 30.0f;
            jaw.vx = (float)GetRandomValue(-20, 20);
            jaw.vy = (float)GetRandomValue(-100, -20);
            jaw.rotation = 0;
            jaw.rotSpeed = (float)GetRandomValue(-200, 200);
            jaw.timer = 1.0f;
            jaw.active = true;
            m_fallingParts.push_back(jaw);

            FallingPart hair;
            hair.texture = res.GetTexture("ZOMBIE_HAIR");
            hair.x = m_x + 60.0f; 
            hair.y = m_y + 0.0f;
            hair.vx = (float)GetRandomValue(-40, 40);
            hair.vy = (float)GetRandomValue(-150, -50);
            hair.rotation = 0;
            hair.rotSpeed = (float)GetRandomValue(-150, 150);
            hair.timer = 1.0f;
            hair.active = true;
            m_fallingParts.push_back(hair);

            if (!m_hasLostArm) {
                m_hasLostArm = true;
                FallingPart arm;
                arm.texture = res.GetTexture("ZOMBIE_OUTERARM_LOWER");
                arm.x = m_x + 30.0f; 
                arm.y = m_y + 50.0f;
                arm.vx = (float)GetRandomValue(-40, 20);
                arm.vy = (float)GetRandomValue(-120, -40);
                arm.rotation = 0;
                arm.rotSpeed = (float)GetRandomValue(-150, 150);
                arm.timer = 1.0f;
                arm.active = true;
                m_fallingParts.push_back(arm);
            }
            if (!m_hasLostCone) {
                m_hasLostCone = true;
                FallingPart cone;
                cone.texture = res.GetTexture("ZOMBIE_CONE1");
                cone.x = m_x + 30.0f; 
                cone.y = m_y + 10.0f;
                cone.vx = (float)GetRandomValue(-30, 30);
                cone.vy = (float)GetRandomValue(-150, -50);
                cone.rotation = 0;
                cone.rotSpeed = (float)GetRandomValue(-150, 150);
                cone.timer = 1.0f;
                cone.active = true;
                m_fallingParts.push_back(cone);
            }
        }
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        if (m_hasLostCone) {
            m_anim.SetTrackVisible("anim_cone", false);
        } else {
            m_anim.SetTrackVisible("anim_cone", true);
        }
        if (m_hasLostArm) {
            m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
            m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
        } else {
            m_anim.SetTrackVisible("Zombie_outerarm_lower", true);
            m_anim.SetTrackVisible("Zombie_outerarm_hand", true);
        }
        if (currentAnim != "anim_death" && currentAnim != "anim_death2") {
            m_hasSpawnedDeathParts = false;
        }
    }

    if (!isDead()) {
        if (currentAnim == "anim_walk" || currentAnim == "anim_walk2" || currentAnim == "anim_slowwalk") {
            m_x -= m_speed * deltaTime;
        }
    }

    for (auto& part : m_fallingParts) {
        if (part.active) {
            part.vy += 400.0f * deltaTime; // gravity
            part.x += part.vx * deltaTime;
            part.y += part.vy * deltaTime;
            part.rotation += part.rotSpeed * deltaTime;
            part.timer -= deltaTime;
            if (part.timer <= 0) {
                part.active = false;
            }
        }
    }
}

void ConeheadZombie::draw() {
    if (m_isDevoured) return;

    if (m_isSquashed) {
        float alpha = std::clamp(1.0f - m_squashTimer / 2.0f, 0.0f, 1.0f);
        rlPushMatrix();
        rlTranslatef(0.0f, m_y + 120.0f, 0.0f);
        rlScalef(1.15f, 0.20f, 1.0f);
        rlTranslatef(0.0f, -(m_y + 120.0f), 0.0f);
        m_anim.Draw(m_x, m_y, 1.0f, ColorAlpha(WHITE, alpha));
        rlPopMatrix();
        return;
    }

    if (m_isCharred) {
        m_charredAnim.Draw(m_x, m_y, 1.0f);
        return;
    }

    m_anim.Draw(m_x, m_y, 1.0f);

    for (const auto& part : m_fallingParts) {
        if (part.active && part.texture.id != 0) {
            Rectangle source = { 0, 0, (float)part.texture.width, (float)part.texture.height };
            Rectangle dest = { part.x, part.y, (float)part.texture.width, (float)part.texture.height };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(part.texture, source, dest, origin, part.rotation, WHITE);
        }
    }
}
