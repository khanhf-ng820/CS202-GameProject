#include "CherryBomb.h"

CherryBomb::CherryBomb(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 150, "CherryBomb") {
    // Load reanim for CherryBomb
    getResources(res.GetAssetPath("assets/reanim/CherryBomb.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

CherryBomb::~CherryBomb() {
}

void CherryBomb::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    if (is_exploding_effect) {
        explosion_timer += deltaTime;
        if (explosion_timer >= 0.5f) { // Hide after 0.5s
            m_hp = 0; // Die
        }
        return;
    }

    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();
    if (currentAnim == "anim_explode") {
        if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
            is_exploding_effect = true;
            explosion_timer = 0.0f;
            did_explode = true;
        }
    } else {
        did_explode = false;
        is_exploding_effect = false;
    }
}

void CherryBomb::draw() {
    if (is_exploding_effect) {
        Texture2D powTex = res.GetTexture("Pow");
        Texture2D powieTex = res.GetTexture("ExplosionPowie");
        
        float progress = explosion_timer / 0.5f; // 0.0 -> 1.0
        
        // Scale out quickly
        float scale = 0.5f + 2.5f * (1.0f - powf(1.0f - progress, 3.0f)); // ease-out cubic
        
        // Fade out
        unsigned char alpha = 255;
        if (progress > 0.5f) {
            alpha = (unsigned char)(255 * (1.0f - progress) / 0.5f);
        }
        Color tint = {255, 255, 255, alpha};

        float cx = m_x + 40.0f; // Approximate center of the plant
        float cy = m_y + 40.0f;

        if (powieTex.id != 0) {
            DrawTexturePro(powieTex, 
                {0, 0, (float)powieTex.width, (float)powieTex.height},
                {cx, cy, powieTex.width * scale * 1.5f, powieTex.height * scale * 1.5f},
                {powieTex.width * scale * 1.5f / 2.0f, powieTex.height * scale * 1.5f / 2.0f},
                progress * 90.0f, // Slight rotation
                tint);
        }

        if (powTex.id != 0) {
            DrawTexturePro(powTex, 
                {0, 0, (float)powTex.width, (float)powTex.height},
                {cx, cy, powTex.width * scale, powTex.height * scale},
                {powTex.width * scale / 2.0f, powTex.height * scale / 2.0f},
                0.0f, 
                tint);
        }
    } else {
        m_anim.Draw(m_x, m_y, 1.0f); 
    }
}
