#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"

class LawnMower {
public:
    LawnMower(Resources& res, float x, float y, int row);
    ~LawnMower() = default;

    void update(float dt);
    void draw() const;

    void trigger();
    bool isTriggered() const { return m_isTriggered; }
    bool isFinished() const { return m_isFinished; }

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    int getRow() const { return m_row; }
    Rectangle getBounds() const { return { m_x, m_y, 70.0f, 60.0f }; }

private:
    Reanimation m_anim;
    float m_x;
    float m_y;
    int m_row;
    bool m_isTriggered = false;
    bool m_isFinished = false;
    float m_speed = 420.0f;
};
