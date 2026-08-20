#include "Level5.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "NewspaperZombie.h"
#include "PoleVaultingZombie.h"
#include "FlagZombie.h"
#include "AudioManager.h"

Level5::Level5(Resources& res, RenderTexture2D targetScreen)
    : Level4(res, targetScreen, 5) {
    m_hasFog = true;
    m_fogStartX = 480.0f; // Fog covers columns 6, 7, 8 (last 3 columns)
    m_maxWaves = 8;
    initGraves();
    initPreviewZombies();
}

std::vector<std::string> Level5::getUniqueLevelZombieTypes() const {
    return { "ZombieNormal", "ConeheadZombie", "BucketheadZombie", "NewspaperZombie", "PoleVaultingZombie", "FlagZombie" };
}

void Level5::initGraves() {
    m_graves.clear();
    m_introGraveSoundPlayed = false;

    // 5 Graves placed across columns 4, 5, 6, 7
    m_graves.push_back({ 0, 6, 0, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 1, 5, 1, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 2, 7, 2, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 3, 4, 3, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 4, 6, 4, 0, false, 0.0f, 0.0f, "" });
}

void Level5::spawnNextWave() {
    m_currentWave++;
    float spawnX = 720.0f;

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 40.0f, laneY(4)));
        m_waveTimer = 18.0f;
    } else if (m_currentWave == 2) {
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 30.0f, laneY(3)));
        m_waveTimer = 19.0f;
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 35.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 60.0f, laneY(4)));
        m_waveTimer = 20.0f;
    } else if (m_currentWave == 4) {
        // Wave 4: Buckethead and Grave Rising!
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 30.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 60.0f, laneY(3)));
        triggerGraveRising(2);
        m_waveTimer = 22.0f;
    } else if (m_currentWave == 5) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 30.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 60.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 80.0f, laneY(4)));
        m_waveTimer = 21.0f;
    } else if (m_currentWave == 6) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 35.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 60.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 90.0f, laneY(3)));
        m_waveTimer = 22.0f;
    } else if (m_currentWave == 7) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 30.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 55.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 80.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 100.0f, laneY(4)));
        m_waveTimer = 23.0f;
    } else if (m_currentWave == 8) {
        // Final Wave (Huge Wave)
        m_finalWaveAnnounced = true;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));

        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 25.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 30.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 50.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 60.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 75.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 85.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 100.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 110.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 120.0f, laneY(4)));

        triggerGraveRising(-1);
    }
}
