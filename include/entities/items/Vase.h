#pragma once
#include "raylib.h"
#include "resources.h"

enum class VaseType {
    Brown,
    Green,
    Zombie
};

enum class VaseState {
    Intact,
    PendingBreak,
    Destroyed
};

class Vase {
protected:
    int m_row;
    int m_col;
    float m_x;
    float m_y;
    VaseType m_type;
    VaseState m_state;

public:
    Vase(int row, int col, float x, float y, VaseType type);
    virtual ~Vase() = default;

    virtual void draw(Resources& res) const;

    int getRow() const { return m_row; }
    int getCol() const { return m_col; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    VaseType getType() const { return m_type; }
    VaseState getState() const { return m_state; }

    bool isTargetable() const { return m_state == VaseState::Intact; }
    void setPendingBreak() { m_state = VaseState::PendingBreak; }
    void destroy() { m_state = VaseState::Destroyed; }
    bool isDestroyed() const { return m_state == VaseState::Destroyed; }

    Rectangle getBounds() const { return { m_x, m_y, 80.0f, 101.0f }; }
};

class BrownVase : public Vase {
public:
    BrownVase(int row, int col, float x, float y);
};

class GreenVase : public Vase {
public:
    GreenVase(int row, int col, float x, float y);
};
