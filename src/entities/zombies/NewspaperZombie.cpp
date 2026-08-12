#include "NewspaperZombie.h"
#include "rlgl.h"
#include <algorithm>

NewspaperZombie::NewspaperZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 350.0f, 8.0f, 100, "NewspaperZombie") {
    
    getResources(res.GetAssetPath("assets/reanim/Zombie_paper.reanim"));
    m_anim.SetBaseAnimation("anim_walk");
    m_anim.SetAnimation("anim_walk");

    m_anim.SetTrackVisible("Zombie_paper_paper", true);
}

NewspaperZombie::~NewspaperZombie() {}

void NewspaperZombie::takeDamage(float damage) {
    if (m_isCharred || m_isSquashed || m_isDevoured) return;

    if (!m_hasLostNewspaper && (m_hp - damage <= 200)) {
        m_hasLostNewspaper = true;
        m_anim.SetTrackVisible("Zombie_paper_paper", false);
        m_isGasping = true;
        m_anim.SetAnimation("anim_gasp");
        m_anim.SetBaseAnimation("anim_gasp");
        m_anim.OverrideTrackImage("anim_head1", "ZOMBIE_PAPER_MADHEAD");
        m_anim.OverrideTrackImage("anim_head_look", "ZOMBIE_PAPER_MADHEAD");

        Resources& res = Resources::GetInstance();
        
        FallingPart paper1;
        paper1.texture = res.GetTexture("ZOMBIE_PAPER_PAPER2");
        paper1.x = m_x + 20.0f;
        paper1.y = m_y + 40.0f;
        paper1.vx = (float)GetRandomValue(-40, 40);
        paper1.vy = (float)GetRandomValue(-150, -50);
        paper1.rotation = 0;
        paper1.rotSpeed = (float)GetRandomValue(-200, 200);
        paper1.timer = 1.0f;
        paper1.active = true;
        m_fallingParts.push_back(paper1);

        FallingPart paper2;
        paper2.texture = res.GetTexture("ZOMBIE_PAPER_PAPER3");
        paper2.x = m_x + 40.0f;
        paper2.y = m_y + 30.0f;
        paper2.vx = (float)GetRandomValue(10, 60);
        paper2.vy = (float)GetRandomValue(-150, -50);
        paper2.rotation = 0;
        paper2.rotSpeed = (float)GetRandomValue(-200, 200);
        paper2.timer = 1.0f;
        paper2.active = true;
        m_fallingParts.push_back(paper2);

        FallingPart glasses;
        glasses.texture = res.GetTexture("ZOMBIE_PAPER_GLASSES");
        glasses.x = m_x + 50.0f;
        glasses.y = m_y + 10.0f;
        glasses.vx = (float)GetRandomValue(-20, 20);
        glasses.vy = (float)GetRandomValue(-100, -50);
        glasses.rotation = 0;
        glasses.rotSpeed = (float)GetRandomValue(-200, 200);
        glasses.timer = 1.0f;
        glasses.active = true;
        m_fallingParts.push_back(glasses);
    }

    Zombie::takeDamage(damage);
    
    if (m_hp <= 0) {
        if (m_anim.GetCurrentAnimName() != "anim_death") {
            m_anim.SetAnimation("anim_death");
        }
    } else if (m_hp <= 100 && !m_hasLostArm) {
        m_hasLostArm = true;
        m_anim.SetTrackVisible("Zombie_paper_leftarm_lower", false);
        m_anim.SetTrackVisible("Zombie_paper_hands", false);
        m_anim.OverrideTrackImage("Zombie_paper_leftarm_upper", "ZOMBIE_PAPER_LEFTARM_UPPER2");
        Resources& res = Resources::GetInstance();
        FallingPart arm;
        arm.texture = res.GetTexture("ZOMBIE_PAPER_LEFTARM_LOWER");
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

void NewspaperZombie::update(float deltaTime) {
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
        m_anim.SetTrackVisible("anim_head_look", false);
        m_anim.SetTrackVisible("anim_head_pupils", false);
        m_anim.SetTrackVisible("anim_hairpiece", false);
        m_anim.SetTrackVisible("anim_head_jaw", false);
        m_anim.SetTrackVisible("anim_head_glasses", false);
        m_anim.SetTrackVisible("Zombie_paper_leftarm_lower", false);
        m_anim.SetTrackVisible("Zombie_paper_hands", false);
        m_anim.OverrideTrackImage("Zombie_paper_leftarm_upper", "ZOMBIE_PAPER_LEFTARM_UPPER2");

        if (!m_hasSpawnedDeathParts) {
            m_hasSpawnedDeathParts = true;
            Resources& res = Resources::GetInstance();

            FallingPart head;
            head.texture = res.GetTexture("ZOMBIE_PAPER_MADHEAD");
            if (!m_hasLostNewspaper) {
                head.texture = res.GetTexture("ZOMBIE_PAPER_HEAD_LOOK");
            }
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
                arm.texture = res.GetTexture("ZOMBIE_PAPER_LEFTARM_LOWER");
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
        }
    } else {
        if (m_hasLostNewspaper) {
            m_anim.SetTrackVisible("anim_head_glasses", false);
        } else {
            m_anim.SetTrackVisible("anim_head_look", true);
            m_anim.SetTrackVisible("anim_head_glasses", true);
        }
        
        m_anim.SetTrackVisible("anim_head1", true);
        if (m_hasLostArm) {
            m_anim.SetTrackVisible("Zombie_paper_leftarm_lower", false);
            m_anim.SetTrackVisible("Zombie_paper_hands", false);
            m_anim.OverrideTrackImage("Zombie_paper_leftarm_upper", "ZOMBIE_PAPER_LEFTARM_UPPER2");
        } else {
            m_anim.SetTrackVisible("Zombie_paper_leftarm_lower", true);
            m_anim.SetTrackVisible("Zombie_paper_hands", true);
        }
        if (currentAnim != "anim_death") {
            m_hasSpawnedDeathParts = false;
        }
    }

    if (!isDead()) {
        if (m_isGasping) {
            if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 2) {
                m_isGasping = false;
                m_speed = 20.0f;
                m_anim.SetAnimation("anim_walk_nopaper");
                m_anim.SetBaseAnimation("anim_walk_nopaper");
                m_anim.SetSpeed(2.5f);
            }
        } else {
            if (currentAnim == "anim_walk" || currentAnim == "anim_walk_nopaper") {
                m_x -= m_speed * deltaTime;
            }
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

void NewspaperZombie::draw() {
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
