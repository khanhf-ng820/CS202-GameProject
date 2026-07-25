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
    int m_hp;              // Máu hiện tại
    int m_maxHp;           // Máu tối đa
    float m_speed;         // Tốc độ di chuyển
    int m_damage;          // Sát thương gây ra cho cây
    std::string m_name;    // Tên zombie
    Reanimation m_anim;    // Animation của zombie
    bool m_isEating;       // Trạng thái đang ăn cây

public:
    Zombie(Resources& res, float x, float y, int hp, float speed, int damage, std::string name);
    virtual ~Zombie() {}

    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;

    void getResources(std::string dir);

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    int getHp() const { return m_hp; }
    float getSpeed() const { return m_speed; }
    int getDamage() const { return m_damage; }
    std::string getName() const { return m_name; }
    Reanimation& getAnim() { return m_anim; }

    virtual bool isDead() const { return m_hp <= 0; }
    virtual bool isFinished() const { return m_hp <= 0; }
    virtual void takeDamage(int damage) { m_hp -= damage; }
    
    void setEating(bool isEating) { m_isEating = isEating; }
    bool isEating() const { return m_isEating; }
};
