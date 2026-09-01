#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "AudioManager.h"
#include <string>

struct FallingPart {
    Texture2D texture;
    float x, y;
    float vx, vy;
    float rotation;
    float rotSpeed;
    float timer;
    bool active;
};

class Zombie {
protected:
    Resources& res;
    float m_x, m_y;        // Tọa độ của zombie (dùng float cho m_x để di chuyển mượt)
    float m_hp;            // Máu hiện tại (dùng float để nhận sát thương liên tục chính xác)
    float m_maxHp;         // Máu tối đa
    float m_speed;         // Tốc độ di chuyển
    int m_damage;          // Sát thương gây ra cho cây
    std::string m_name;    // Tên zombie
    Reanimation m_anim;    // Animation của zombie
    bool m_isEating;       // Trạng thái đang ăn cây
    float m_eatTimer{ 0.0f }; // Timer đếm ngược/tích lũy để tạo vụn cây khi đang ăn

    bool m_isCharred{ false };
    Reanimation m_charredAnim;
    float m_charredTimer{ 0.0f };
    float m_deathTimer{ 0.0f };

    bool m_isSquashed{ false };
    float m_squashTimer{ 0.0f };
    bool m_isDevoured{ false };

    bool m_isSlowed{ false };
    float m_slowTimer{ 0.0f };

    bool m_isBitingGarlic{ false };
    float m_garlicBiteTimer{ 0.0f };
    bool m_isChangingLane{ false };
    float m_targetLaneY{ 0.0f };

public:
    Zombie(Resources& res, float x, float y, int hp, float speed, int damage, std::string name);
    virtual ~Zombie() {}

    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;

    void getResources(std::string dir);

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setY(float y) { m_y = y; }
    void setX(float x) { m_x = x; }
    float getHp() const { return m_hp; }
    float getSpeed() const { return m_speed; }
    int getDamage() const { return m_damage; }
    std::string getName() const { return m_name; }
    Reanimation& getAnim() { return m_anim; }

    bool isBitingGarlic() const { return m_isBitingGarlic; }
    bool isChangingLane() const { return m_isChangingLane; }
    float getTargetLaneY() const { return m_targetLaneY; }

    void startBitingGarlic(float targetLaneY, float biteDuration = 0.55f) {
        m_isBitingGarlic = true;
        m_garlicBiteTimer = biteDuration;
        m_targetLaneY = targetLaneY;
        m_isEating = true;
        m_anim.SetAnimation("anim_eat");
    }

    void updateGarlicBite(float dt) {
        if (m_isBitingGarlic) {
            m_garlicBiteTimer -= dt;
            if (m_garlicBiteTimer <= 0.0f) {
                m_isBitingGarlic = false;
                m_isEating = false;
                m_isChangingLane = true;
                m_anim.SetAnimation("anim_walk");

                std::string sfx = (GetRandomValue(0, 1) == 0)
                    ? res.GetAssetPath("assets/sounds/yuck.ogg")
                    : res.GetAssetPath("assets/sounds/yuck2.ogg");
                AudioManager::GetInstance().PlaySoundEffect(sfx);
            }
        }
        if (m_isChangingLane) {
            float shiftSpeed = 75.0f; // px/s
            if (m_targetLaneY > m_y) {
                m_y = std::min(m_targetLaneY, m_y + shiftSpeed * dt);
            } else {
                m_y = std::max(m_targetLaneY, m_y - shiftSpeed * dt);
            }
            if (std::abs(m_y - m_targetLaneY) < 0.5f) {
                m_y = m_targetLaneY;
                m_isChangingLane = false;
            }
        }
    }

    void applySlow(float duration = 3.0f) {
        m_isSlowed = true;
        m_slowTimer = duration;
    }
    bool isSlowed() const { return m_isSlowed; }
    float getSlowTimer() const { return m_slowTimer; }

    void updateSlow(float dt) {
        if (m_isSlowed) {
            m_slowTimer -= dt;
            if (m_slowTimer <= 0.0f) {
                m_slowTimer = 0.0f;
                m_isSlowed = false;
            }
        }
    }

    Color getDrawTint() const {
        if (m_isSlowed) {
            return Color{ 120, 190, 255, 255 }; // Light ice-blue tint
        }
        return WHITE;
    }

    virtual bool isDead() const { return m_hp <= 0; }
    virtual bool isFinished() const {
        if (m_isDevoured) {
            return true;
        }
        if (m_isSquashed) {
            return m_squashTimer >= 2.0f;
        }
        if (m_isCharred) {
            return m_charredTimer >= 1.5f;
        }
        if (m_hp <= 0) {
            std::string animName = m_anim.GetCurrentAnimName();
            if (animName == "anim_death" || animName == "anim_death2" || animName == "anim_waterdeath") {
                return m_deathTimer >= 2.2f;
            }
            return true;
        }
        return false;
    }
    virtual void takeDamage(float damage) { m_hp -= damage; }
    virtual void takeExplosiveDamage(float damage);
    virtual void takeSquashDamage(float damage);
    virtual void devour() { m_hp = 0; m_isDevoured = true; }
    bool isCharred() const { return m_isCharred; }
    bool isSquashed() const { return m_isSquashed; }
    bool isDevoured() const { return m_isDevoured; }
    
    void setEating(bool isEating) { m_isEating = isEating; }
    bool isEating() const { return m_isEating; }
    float getEatTimer() const { return m_eatTimer; }
    void addEatTimer(float dt) { m_eatTimer += dt; }
    void resetEatTimer() { m_eatTimer = 0.0f; }
};
