#include "Level3.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "NewspaperZombie.h"
#include "PoleVaultingZombie.h"
#include "FootballZombie.h"
#include "FlagZombie.h"
#include "AudioManager.h"

Level3::Level3(Resources& res, RenderTexture2D targetScreen)
    : Level1(res, targetScreen, 3) {
    m_maxWaves = 10;
    initPreviewZombies();
}

std::vector<std::string> Level3::getUniqueLevelZombieTypes() const {
    return {
        "ZombieNormal",
        "ConeheadZombie",
        "BucketheadZombie",
        "NewspaperZombie",
        "PoleVaultingZombie",
        "FootballZombie",
        "FlagZombie"
    };
}

void Level3::spawnNextWave() {
    m_currentWave++;
    float spawnX = 700.0f;

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    bool isHugeWave = false;

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(1)));
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(3)));
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 5) {
        isHugeWave = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
    } else if (m_currentWave == 6) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(4)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(0)));
    } else if (m_currentWave == 7) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
    } else if (m_currentWave == 8) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(0)));
    } else if (m_currentWave == 9) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(4)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
    } else if (m_currentWave == 10) {
        isHugeWave = true;
        m_finalWaveAnnounced = true;
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
    }

    if (isHugeWave) {
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));
    } else {
        static const std::vector<std::string> waveGroanSounds = {
            "assets/sounds/sukhbir.ogg", "assets/sounds/sukhbir2.ogg", "assets/sounds/sukhbir3.ogg",
            "assets/sounds/groan.ogg", "assets/sounds/groan2.ogg", "assets/sounds/lowgroan.ogg"
        };
        int rIdx = GetRandomValue(0, (int)waveGroanSounds.size() - 1);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(waveGroanSounds[rIdx]));
    }
}
