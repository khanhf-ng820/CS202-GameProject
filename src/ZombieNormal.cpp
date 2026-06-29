#include "ZombieNormal.h"

ZombieNormal::ZombieNormal(Resources& res, float x, float y)
    : Zombie(res, x, y, 200, 25.0f, 100, "ZombieNormal") {
    
    getResources(res.GetAssetPath("assets/reanim/Zombie.reanim"));
    m_anim.SetBaseAnimation("anim_walk");

    // Ẩn tất cả các phụ kiện để trở thành Zombie cơ bản
    m_anim.SetTrackVisible("anim_bucket", false);
    m_anim.SetTrackVisible("anim_cone", false);
    m_anim.SetTrackVisible("anim_screendoor", false);
    m_anim.SetTrackVisible("Zombie_duckytube", false);
    m_anim.SetTrackVisible("Zombie_mustache", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
    m_anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_flaghand", false);
}

ZombieNormal::~ZombieNormal() {}

void ZombieNormal::update(float deltaTime) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    // Ẩn đầu khi chạy animation chết
    if (currentAnim == "anim_death" || currentAnim == "anim_death2" || currentAnim == "anim_waterdeath") {
        m_anim.SetTrackVisible("anim_head1", false);
        m_anim.SetTrackVisible("Zombie_outerarm_lower", false);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", false);

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
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        m_anim.SetTrackVisible("Zombie_outerarm_lower", true);
        m_anim.SetTrackVisible("Zombie_outerarm_hand", true);
        if (currentAnim != "anim_death" && currentAnim != "anim_death2") {
            m_hasSpawnedDeathParts = false;
        }
    }

    if (!isDead()) {
        // Chỉ di chuyển khi đang ở animation walk
        if (currentAnim == "anim_walk" || currentAnim == "anim_walk2") {
            m_x -= m_speed * deltaTime;
        }
    }

    // Cập nhật các bộ phận rơi
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

void ZombieNormal::draw() {
    m_anim.Draw(m_x, m_y, 1.6f);

    for (const auto& part : m_fallingParts) {
        if (part.active && part.texture.id != 0) {
            Rectangle source = { 0, 0, (float)part.texture.width, (float)part.texture.height };
            Rectangle dest = { part.x, part.y, part.texture.width * 1.6f, part.texture.height * 1.6f };
            Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };
            DrawTexturePro(part.texture, source, dest, origin, part.rotation, WHITE);
        }
    }
}
