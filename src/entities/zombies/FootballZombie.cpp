#include "FootballZombie.h"
#include "rlgl.h"
#include <algorithm>

FootballZombie::FootballZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 1670, 20.0f, 100, "FootballZombie") {
    
    getResources(res.GetAssetPath("assets/reanim/Zombie_football.reanim"));
    m_anim.SetBaseAnimation("anim_walk");
    m_anim.SetAnimation("anim_walk");
    m_anim.SetTrackVisible("zombie_football_helmet", true);

    // Bake inherited rotation from parent leg to child foot for the anim_eat sequence
    // to fix the detached foot bug caused by missing global transforms in the .reanim asset.
    m_anim.BakeChildRotation("zombie_football_rightleg_lower", "zombie_football_rightleg_foot", "anim_eat");
    m_anim.BakeChildRotation("zombie_football_leftleg_lower", "zombie_football_leftleg_foot", "anim_eat");
}

FootballZombie::~FootballZombie() {}

void FootballZombie::takeDamage(float damage) {
    if (m_isCharred || m_isSquashed || m_isDevoured) return;

    if (!m_hasLostHelmet && (m_hp - damage <= 270)) {
        m_hasLostHelmet = true;
        m_anim.SetTrackVisible("zombie_football_helmet", false);
        Resources& res = Resources::GetInstance();
        FallingPart helmet;
        helmet.texture = res.GetTexture("ZOMBIE_FOOTBALL_HELMET3");
        helmet.x = m_x + 30.0f; 
        helmet.y = m_y + 10.0f;
        helmet.vx = (float)GetRandomValue(-30, 30);
        helmet.vy = (float)GetRandomValue(-150, -50);
        helmet.rotation = 0;
        helmet.rotSpeed = (float)GetRandomValue(-150, 150);
        helmet.timer = 1.0f;
        helmet.active = true;
        m_fallingParts.push_back(helmet);
    }

    Zombie::takeDamage(damage);
    if (m_hp <= 0) {
        if (m_anim.GetCurrentAnimName() != "anim_death") {
            m_anim.SetAnimation("anim_death");
        }
    } else if (m_hp <= 135 && !m_hasLostArm) {
        m_hasLostArm = true;
        Resources& res = Resources::GetInstance();
        FallingPart arm;
        arm.texture = res.GetTexture("ZOMBIE_FOOTBALL_LEFTARM_LOWER");
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

void FootballZombie::update(float deltaTime) {
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

    if (!m_hasLostHelmet) {
        if (m_hp <= 730) {
            m_anim.OverrideTrackImage("zombie_football_helmet", "Zombie_football_helmet3");
        } else if (m_hp <= 1190) {
            m_anim.OverrideTrackImage("zombie_football_helmet", "Zombie_football_helmet2");
        } else {
            m_anim.OverrideTrackImage("zombie_football_helmet", "Zombie_football_helmet");
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

    if (currentAnim == "anim_death") {
        m_anim.SetTrackVisible("anim_head1", false);
        m_anim.SetTrackVisible("anim_hair", false);
        m_anim.SetTrackVisible("anim_head2", false);
        m_anim.SetTrackVisible("zombie_football_leftarm_lower", false);
        m_anim.SetTrackVisible("zombie_football_leftarm_hand", false);
        m_anim.SetTrackVisible("zombie_football_helmet", false);

        if (!m_hasSpawnedDeathParts) {
            m_hasSpawnedDeathParts = true;
            Resources& res = Resources::GetInstance();

            FallingPart head;
            head.texture = res.GetTexture("ZOMBIEFOOTBALLHEAD");
            head.x = m_x + 60.0f; 
            head.y = m_y + 10.0f;
            head.vx = (float)GetRandomValue(-30, 30);
            head.vy = (float)GetRandomValue(-150, -50);
            head.rotation = 0;
            head.rotSpeed = (float)GetRandomValue(-100, 100);
            head.timer = 1.0f;
            head.active = true;
            m_fallingParts.push_back(head);

            if (!m_hasLostArm) {
                m_hasLostArm = true;
                FallingPart arm;
                arm.texture = res.GetTexture("ZOMBIE_FOOTBALL_LEFTARM_LOWER");
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
            if (!m_hasLostHelmet) {
                m_hasLostHelmet = true;
                FallingPart helmet;
                helmet.texture = res.GetTexture("ZOMBIE_FOOTBALL_HELMET3");
                helmet.x = m_x + 30.0f; 
                helmet.y = m_y + 10.0f;
                helmet.vx = (float)GetRandomValue(-30, 30);
                helmet.vy = (float)GetRandomValue(-150, -50);
                helmet.rotation = 0;
                helmet.rotSpeed = (float)GetRandomValue(-150, 150);
                helmet.timer = 1.0f;
                helmet.active = true;
                m_fallingParts.push_back(helmet);
            }
        }
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        if (m_hasLostHelmet) {
            m_anim.SetTrackVisible("zombie_football_helmet", false);
        } else {
            m_anim.SetTrackVisible("zombie_football_helmet", true);
        }
        if (m_hasLostArm) {
            m_anim.SetTrackVisible("zombie_football_leftarm_lower", false);
            m_anim.SetTrackVisible("zombie_football_leftarm_hand", false);
        } else {
            m_anim.SetTrackVisible("zombie_football_leftarm_lower", true);
            m_anim.SetTrackVisible("zombie_football_leftarm_hand", true);
        }
        if (currentAnim != "anim_death") {
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

void FootballZombie::draw() {
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
