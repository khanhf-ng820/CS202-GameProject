#include "Level6.h"
#include "ZombieNormal.h"
#include "ConeheadZombie.h"
#include "BucketheadZombie.h"
#include "NewspaperZombie.h"
#include "PoleVaultingZombie.h"
#include "FootballZombie.h"
#include "FlagZombie.h"
#include "AudioManager.h"

Level6::Level6(Resources& res, RenderTexture2D targetScreen)
    : Level4(res, targetScreen, 6) {
    m_hasFog = true;
    m_fogStartX = 410.0f; // Dense fog encroaches deeper into column 5 (last 4 columns)
    m_baseFogStartX = 410.0f;
    for (int r = 0; r < 5; ++r) {
        m_rowFogStartX[r] = m_baseFogStartX;
        m_targetRowFogStartX[r] = m_baseFogStartX;
    }
    m_maxWaves = 10;
    initGraves();
    initPreviewZombies();
}

std::vector<std::string> Level6::getUniqueLevelZombieTypes() const {
    return { "ZombieNormal", "ConeheadZombie", "BucketheadZombie", "NewspaperZombie", "PoleVaultingZombie", "FootballZombie", "FlagZombie" };
}

void Level6::initGraves() {
    m_graves.clear();
    m_introGraveSoundPlayed = false;

    // 6 Graves buried in deep night fog
    m_graves.push_back({ 0, 5, 0, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 1, 6, 1, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 2, 4, 2, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 2, 7, 3, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 3, 5, 4, 0, false, 0.0f, 0.0f, "" });
    m_graves.push_back({ 4, 7, 0, 0, false, 0.0f, 0.0f, "" });
}

void Level6::spawnNextWave() {
    m_currentWave++;
    float spawnX = 720.0f;

    auto laneY = [](int row) -> float {
        return 45.0f + row * 100.0f;
    };

    if (m_currentWave == 1) {
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 30.0f, laneY(0)));
        m_waveTimer = 17.0f;
    } else if (m_currentWave == 2) {
        // Early fast Football Zombie surprise!
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 40.0f, laneY(1)));
        m_waveTimer = 18.0f;
    } else if (m_currentWave == 3) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 30.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 60.0f, laneY(4)));
        m_waveTimer = 19.0f;
    } else if (m_currentWave == 4) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 35.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 70.0f, laneY(2)));
        m_waveTimer = 20.0f;
    } else if (m_currentWave == 5) {
        // Mid-Level Huge Wave 1!
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));

        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 25.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 45.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 65.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 75.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ZombieNormal>(res, spawnX + 95.0f, laneY(2)));

        triggerGraveRising(3);
        m_waveTimer = 24.0f;
    } else if (m_currentWave == 6) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 35.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 50.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 75.0f, laneY(1)));
        m_waveTimer = 21.0f;
    } else if (m_currentWave == 7) {
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 30.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 50.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 70.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 85.0f, laneY(4)));
        m_waveTimer = 22.0f;
    } else if (m_currentWave == 8) {
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX, laneY(1)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 25.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 45.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 65.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 75.0f, laneY(4)));
        m_waveTimer = 23.0f;
    } else if (m_currentWave == 9) {
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX, laneY(0)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 25.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 40.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 60.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 75.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 90.0f, laneY(2)));
        m_waveTimer = 24.0f;
    } else if (m_currentWave == 10) {
        // Finale Huge Wave Horde
        m_finalWaveAnnounced = true;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/hugewave.ogg"));
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/siren.ogg"));

        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX, laneY(2)));
        m_zombies.push_back(std::make_unique<FlagZombie>(res, spawnX + 20.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 30.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 40.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<FootballZombie>(res, spawnX + 50.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 65.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 75.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<BucketheadZombie>(res, spawnX + 85.0f, laneY(4)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 95.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<PoleVaultingZombie>(res, spawnX + 105.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 115.0f, laneY(0)));
        m_zombies.push_back(std::make_unique<NewspaperZombie>(res, spawnX + 125.0f, laneY(2)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 135.0f, laneY(1)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 145.0f, laneY(3)));
        m_zombies.push_back(std::make_unique<ConeheadZombie>(res, spawnX + 155.0f, laneY(4)));

        triggerGraveRising(-1);
    }
}
