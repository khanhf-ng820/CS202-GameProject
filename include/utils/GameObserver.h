#pragma once
#include <string>
#include <vector>
#include <algorithm>

// Observer interface for receiving gameplay event callbacks
class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onSunCollected(int amount) {}
    virtual void onZombieKilled(const std::string& zombieName) {}
    virtual void onPlantDestroyed(int row, int col) {}
};

// Subject manager implementing the Observer Pattern event dispatcher
class GameSubject {
private:
    std::vector<IGameObserver*> m_observers;

public:
    void addObserver(IGameObserver* observer) {
        if (observer && std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
            m_observers.push_back(observer);
        }
    }

    void removeObserver(IGameObserver* observer) {
        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
    }

    void notifySunCollected(int amount) {
        for (auto* obs : m_observers) {
            if (obs) obs->onSunCollected(amount);
        }
    }

    void notifyZombieKilled(const std::string& zombieName) {
        for (auto* obs : m_observers) {
            if (obs) obs->onZombieKilled(zombieName);
        }
    }

    void notifyPlantDestroyed(int row, int col) {
        for (auto* obs : m_observers) {
            if (obs) obs->onPlantDestroyed(row, col);
        }
    }
};

// Concrete subscriber handling audio and game event triggers
class AudioGameObserver : public IGameObserver {
public:
    void onSunCollected(int amount) override {}
    void onZombieKilled(const std::string& zombieName) override {}
};
