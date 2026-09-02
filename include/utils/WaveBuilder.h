#pragma once
#include "Zombie.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "FlagZombie.h"
#include "FootballZombie.h"
#include "NewspaperZombie.h"
#include "PoleVaultingZombie.h"
#include <vector>
#include <memory>

// Builder Pattern implementation for constructing zombie waves fluently
class ZombieWaveBuilder {
private:
    std::vector<std::unique_ptr<Zombie>> m_zombies;
    Resources& m_res;

public:
    explicit ZombieWaveBuilder(Resources& res)
        : m_res(res) {}

    ZombieWaveBuilder& addNormalZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addConeheadZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addBucketheadZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addFlagZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<FlagZombie>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addFootballZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<FootballZombie>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addNewspaperZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<NewspaperZombie>(m_res, x, y));
        return *this;
    }

    ZombieWaveBuilder& addPoleVaultingZombie(float x, float y) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(m_res, x, y));
        return *this;
    }

    std::vector<std::unique_ptr<Zombie>> build() {
        return std::move(m_zombies);
    }
};
