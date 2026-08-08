#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
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
