#pragma once
#include "raylib.h"
#include <cmath>
#include <iostream>
#include "particle.h"

class Projectile {
private:
    float m_x, m_y;
    float m_startX, m_startY;
    float m_speed;
    Texture2D m_tex;
    bool m_active;
    bool m_isSnow;
    bool m_isLobbed;
    float m_range;
    float m_maxHeight;
    float m_scale;
    int m_damage;
    ParticleEffect efftrailing;
    Resources* m_res;

public:
    Projectile(float x, float y, float speed, Texture2D tex, bool isSnow = false, bool isLobbed = false, float scale = 1.0f, int damage = 20, Resources* res = nullptr) 
        : m_x(x), m_y(y), m_startX(x), m_startY(y), m_speed(speed), m_tex(tex), m_active(true), m_isSnow(isSnow), m_isLobbed(isLobbed), m_scale(scale), m_damage(damage), m_res(res ? res : &Resources::GetInstance()) {
        m_range = 500.0f;     // Tầm bắn xa của đạn cầu vồng
        m_maxHeight = 150.0f; // Chiều cao tối đa vòng cung
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

    void draw() const;
    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    int getDamage() const { return m_damage; }
    bool isSnow() const { return m_isSnow; }
    bool isLobbed() const { return m_isLobbed; }

    Resources* getResources() const { return m_res; }
    Resources& getRes() const { return *m_res; }

    ParticleEffect& getTrailingEffect() { return efftrailing; }
    void setTrailingEffect(const ParticleEffect& eff) { efftrailing = eff; }
};

