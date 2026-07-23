#include "FlagZombie.h"

FlagZombie::FlagZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 200, 8.0f, 100, "FlagZombie") {
    getResources(res.GetAssetPath("assets/reanim/Zombie.reanim"));
    m_anim.SetBaseAnimation("anim_walk");
    m_anim.SetAnimation("anim_walk");

    // Hide other zombie accessories
    m_anim.SetTrackVisible("anim_bucket", false);
    m_anim.SetTrackVisible("anim_cone", false);
    m_anim.SetTrackVisible("anim_screendoor", false);
    m_anim.SetTrackVisible("Zombie_duckytube", false);
    m_anim.SetTrackVisible("Zombie_mustache", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
    m_anim.SetTrackVisible("Zombie_outerarm_screendoor", false);

    m_anim.SetTrackVisible("Zombie_flaghand", true);
    m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
}

FlagZombie::~FlagZombie() {}

void FlagZombie::takeDamage(int damage) {
    Zombie::takeDamage(damage);
    if (m_hp <= 0) {
        if (m_anim.GetCurrentAnimName() != "anim_death" && m_anim.GetCurrentAnimName() != "anim_death2") {
            m_anim.SetAnimation("anim_death2");
        }
    }
}

void FlagZombie::update(float deltaTime) {
    m_anim.Update(deltaTime);

    if (m_hp <= 0) {
        m_deathTimer += deltaTime;
    }

    std::string currentAnim = m_anim.GetCurrentAnimName();

    if (currentAnim == "anim_death" || currentAnim == "anim_death2" || currentAnim == "anim_waterdeath") {
        m_anim.SetTrackVisible("anim_head1", false);
        m_anim.SetTrackVisible("anim_hair", false);
        m_anim.SetTrackVisible("anim_head2", false);
        m_anim.SetTrackVisible("anim_tongue", false);
        m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
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
        }
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        m_anim.SetTrackVisible("Zombie_flaghand", true);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", false);
    }

    if (!isDead()) {
        if (currentAnim == "anim_walk" || currentAnim == "anim_walk2" || currentAnim == "anim_slowwalk") {
            m_x -= m_speed * deltaTime;
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
