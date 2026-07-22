#pragma once
#include "raylib.h"

class SunItem {
private:
    float m_x, m_y;
    float m_startX, m_startY;
    float m_targetY;
    Texture2D m_tex;
    float m_lifetime;
    bool m_active;
    bool m_isSkySun;

public:
    SunItem(float x, float y, Texture2D tex);
    SunItem(float x, float startY, float targetY, Texture2D tex);
    ~SunItem();

    void update(float dt);
    void draw() const;
    bool isActive() const;
    bool isClicked(Vector2 mousePos) const;
    void collect();
};
