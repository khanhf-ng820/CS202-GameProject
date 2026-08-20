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

    // Collection flight animation
    bool m_isCollecting;
    float m_collectStartX, m_collectStartY;
    float m_destX, m_destY;
    float m_collectTimer;
    float m_collectDuration;
    bool m_arrived;
    bool m_arrivalHandled;

public:
    SunItem(float x, float y, Texture2D tex);
    SunItem(float x, float startY, float targetY, Texture2D tex);
    ~SunItem();

    void update(float dt);
    void draw() const;
    bool isActive() const;
    bool isClicked(Vector2 mousePos) const;
    bool isCollecting() const { return m_isCollecting; }
    void collect(float destX = 20.0f, float destY = 15.0f);
    bool hasArrived();
};

