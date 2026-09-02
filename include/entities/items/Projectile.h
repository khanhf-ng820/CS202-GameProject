#pragma once
#include "raylib.h"
#include <cmath>
#include <iostream>
#include "particle.h"
#include "Reanimation.h"

#include "TrajectoryStrategy.h"

class Projectile {
private:
    float m_x, m_y;
    float m_startX, m_startY;
    float m_speed;
    Texture2D m_tex;
    bool m_active;
    bool m_isSnow;
    bool m_isFire = false;
    bool m_isMelon = false;
    bool m_isButter = false;
    bool m_isLobbed;
    float m_range;
    float m_maxHeight;
    float m_progress = 0.0f;
    std::shared_ptr<ITrajectoryStrategy> m_strategy;
    float m_scale;
    int m_damage;
    ParticleEffect efftrailing;
    Resources* m_res;
    Reanimation m_fireAnim;
    bool m_hasReanim = false;
    Reanimation m_impactAnim;
    bool m_hasImpactAnim = false;
    bool m_isImpacting = false;
    std::vector<ParticleEffect> m_sparks;
    int m_lastTorchwoodCol = -1;

public:
    Projectile(float x, float y, float speed, Texture2D tex, bool isSnow = false, bool isLobbed = false, float scale = 1.0f, int damage = 20, Resources* res = nullptr, float range = 0) 
        : m_x(x), m_y(y), m_startX(x), m_startY(y), m_speed(speed), m_tex(tex), m_active(true), m_isSnow(isSnow), m_isLobbed(isLobbed), m_scale(scale), m_damage(damage), m_res(res ? res : &Resources::GetInstance()) {
        m_range = range;      // Tầm bắn xa của đạn cầu vồng
        m_maxHeight = 150.0f; // Chiều cao tối đa vòng cung
        if (m_isLobbed) {
            m_strategy = std::make_shared<LobbedTrajectoryStrategy>();
        } else {
            m_strategy = std::make_shared<StraightTrajectoryStrategy>();
        }
        efftrailing.setActive(true);
        if (isSnow && m_res) {
            efftrailing.setTexture(m_res->GetTexture("SnowFlakes"));
            efftrailing.setTotalFrames(3);
            efftrailing.setLoop(true);
        }
    }

    Projectile(Resources& res, float x, float y, float speed, Texture2D tex, bool isSnow = false, bool isLobbed = false, float scale = 1.0f, int damage = 20)
        : Projectile(x, y, speed, tex, isSnow, isLobbed, scale, damage, &res) {}

    void update(float dt);

    void setrange(float range) { m_range = range; }
    void draw() const;
    bool isActive() const { return m_active; }
    bool isImpacting() const { return m_isImpacting; }
    void onHit();
    void deactivate() { m_active = false; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getStartY() const { return m_startY; }
    int getDamage() const { return m_damage; }
    bool isSnow() const { return m_isSnow; }
    bool isFire() const { return m_isFire; }
    bool isMelon() const { return m_isMelon; }
    void setMelon(bool is) { m_isMelon = is; }
    bool isButter() const { return m_isButter; }
    void setButter(bool is) { m_isButter = is; }
    bool isLobbed() const { return m_isLobbed; }
    int getLastTorchwoodCol() const { return m_lastTorchwoodCol; }
    void setLastTorchwoodCol(int col) { m_lastTorchwoodCol = col; }

    void setFire(bool is) {
        if (is) {
            this->ignite(m_res ? m_res->GetTexture("fire1") : Texture2D{0});
        }
    }

    void ignite(Texture2D fireTex = {0}) {
        if (!m_isFire) {
            m_isFire = true;
            m_isSnow = false;
            efftrailing.setActive(false);
            if (fireTex.id != 0) {
                m_tex = fireTex;
            }
            m_damage = 40;
            m_speed = 450.0f;
            if (m_res) {
                m_fireAnim.SetResources(m_res->LoadReanim(m_res->GetAssetPath("assets/reanim/FirePea.reanim")), *m_res);
                m_hasReanim = true;
            }
        }
    }

    void melt() {
        if (m_isSnow) {
            m_isSnow = false;
            m_isFire = false;
            m_damage = 20;
            m_speed = 400.0f;
            efftrailing.setActive(false);
            if (m_res) {
                m_tex = m_res->GetTexture("ProjectilePea");
            }
        }
    }

    Resources* getResources() const { return m_res; }
    Resources& getRes() const { return *m_res; }

    ParticleEffect& getTrailingEffect() { return efftrailing; }
    void setTrailingEffect(const ParticleEffect& eff) { efftrailing = eff; }
};

