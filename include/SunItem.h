#pragma once
#include "raylib.h"

class SunItem {
private:
    float m_x, m_y;
    float m_startX, m_startY;
    Texture2D m_tex;
    float m_lifetime;
    bool m_active;

public:
    SunItem(float x, float y, Texture2D tex);
    ~SunItem();

    void update(float dt);
    void draw() const;
    bool isActive() const;
};
