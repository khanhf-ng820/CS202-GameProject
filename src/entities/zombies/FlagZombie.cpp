#include "FlagZombie.h"
#include "rlgl.h"
#include <algorithm>

FlagZombie::FlagZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 300, 9.33f, 100, "FlagZombie") {
    getResources(res.GetAssetPath("assets/reanim/Zombie.reanim"));
    m_anim.SetBaseAnimation("anim_walk");
    m_anim.SetAnimation("anim_walk");

    // Load animated brain flag and flagpole
    m_flagAnim.SetResources(res.LoadReanim(res.GetAssetPath("assets/reanim/Zombie_flagpole.reanim")), res);

    // Hide other zombie accessories
    m_anim.SetTrackVisible("anim_bucket", false);
    m_anim.SetTrackVisible("anim_cone", false);
    m_anim.SetTrackVisible("anim_screendoor", false);
    m_anim.SetTrackVisible("Zombie_duckytube", false);
    m_anim.SetTrackVisible("Zombie_mustache", false);
    m_anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_whitewater", false);
    m_anim.SetTrackVisible("Zombie_whitewater2", false);

    // Left arm holds the flag: show extended arm and flag hand, hide drooping arm tracks
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor", true);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
    m_anim.SetTrackVisible("anim_innerarm1", false);
    m_anim.SetTrackVisible("anim_innerarm2", false);
    m_anim.SetTrackVisible("anim_innerarm3", false);
    m_anim.SetTrackVisible("Zombie_flaghand", true);

    // Right arm is normal outer arm (all visible)
    m_anim.SetTrackVisible("Zombie_outerarm_upper", true);
    m_anim.SetTrackVisible("Zombie_outerarm_lower", true);
    m_anim.SetTrackVisible("Zombie_outerarm_hand", true);
}

FlagZombie::~FlagZombie() {}

void FlagZombie::takeDamage(float damage) {
    if (m_isCharred || m_isSquashed || m_isDevoured) return;
    Zombie::takeDamage(damage);
    if (m_hp <= 0) {
        if (m_anim.GetCurrentAnimName() != "anim_death" && m_anim.GetCurrentAnimName() != "anim_death2") {
            m_anim.SetAnimation("anim_death2");
        }
    } else if (m_hp <= 100 && !m_hasLostArm) {
        m_hasLostArm = true;
        m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
        m_anim.OverrideTrackImage("Zombie_outerarm_upper", "ZOMBIE_OUTERARM_UPPER2");
        m_flagAnim.OverrideTrackImage("Zombie_flag", "ZOMBIE_FLAG3");
        Resources& res = Resources::GetInstance();
        FallingPart arm;
        arm.texture = res.GetTexture("ZOMBIE_OUTERARM_LOWER");
        arm.x = m_x + 40.0f;
        arm.y = m_y + 40.0f;
        arm.vx = (float)GetRandomValue(-30, 20);
        arm.vy = (float)GetRandomValue(-120, -40);
        arm.rotation = 0;
        arm.rotSpeed = (float)GetRandomValue(-150, 150);
        arm.timer = 1.0f;
        arm.active = true;
        m_fallingParts.push_back(arm);
    }
}

void FlagZombie::update(float deltaTime) {
    if (m_isDevoured) return;
    updateSlow(deltaTime);

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

    float animDt = m_isSlowed ? (deltaTime * 0.5f) : deltaTime;
    m_anim.Update(animDt);

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
        m_anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
        m_anim.SetTrackVisible("Zombie_flaghand", false);

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

            FallingPart flag;
            flag.texture = res.GetTexture("ZOMBIE_FLAGPOLE");
            flag.x = m_x + 20.0f;
            flag.y = m_y + 30.0f;
            flag.vx = (float)GetRandomValue(-40, 40);
            flag.vy = (float)GetRandomValue(-150, -50);
            flag.rotation = 0;
            flag.rotSpeed = (float)GetRandomValue(-150, 150);
            flag.timer = 1.0f;
            flag.active = true;
            m_fallingParts.push_back(flag);
        }
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        m_anim.SetTrackVisible("Zombie_innerarm_screendoor", true);
        m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
        m_anim.SetTrackVisible("anim_innerarm1", false);
        m_anim.SetTrackVisible("anim_innerarm2", false);
        m_anim.SetTrackVisible("anim_innerarm3", false);
        m_anim.SetTrackVisible("Zombie_flaghand", true);
        if (m_hasLostArm) {
            m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
            m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
            m_anim.OverrideTrackImage("Zombie_outerarm_upper", "ZOMBIE_OUTERARM_UPPER2");
        } else {
            m_anim.SetTrackVisible("Zombie_outerarm_lower", true);
            m_anim.SetTrackVisible("Zombie_outerarm_hand", true);
        }
    }

    if (!isDead()) {
        float animDt = m_isSlowed ? (deltaTime * 0.5f) : deltaTime;
        m_flagAnim.Update(animDt);
        if (currentAnim == "anim_walk" || currentAnim == "anim_walk2" || currentAnim == "anim_slowwalk") {
            float moveSpeed = m_isSlowed ? (m_speed * 0.5f) : m_speed;
            m_x -= moveSpeed * deltaTime;
        }
    }

    for (auto& part : m_fallingParts) {
        if (part.active) {
            part.vy += 400.0f * deltaTime;
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

void FlagZombie::draw() {
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

    Color tint = getDrawTint();
    m_anim.Draw(m_x, m_y, 1.0f, tint);
    if (!isDead()) {
        float handX = -9.2f, handY = 50.1f, handRot = 0.0f;
        if (m_anim.GetTrackTransform("Zombie_flaghand", handX, handY, handRot)) {
            float dx = handX - (-9.2f);
            float dy = handY - 50.1f;
            m_flagAnim.Draw(m_x + dx, m_y + dy, 1.0f, tint);
        } else {
            m_flagAnim.Draw(m_x, m_y, 1.0f, tint);
        }
    }

    for (const auto& part : m_fallingParts) {
        if (part.active && part.texture.id != 0) {
            Rectangle source = { 0, 0, (float)part.texture.width, (float)part.texture.height };
            Rectangle dest = { part.x, part.y, (float)part.texture.width, (float)part.texture.height };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(part.texture, source, dest, origin, part.rotation, tint);
        }
    }
}
