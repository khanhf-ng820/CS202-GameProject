#include "ProfileManager.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

static std::string ToLowerStr(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return (char)std::tolower(c);
    });
    return s;
}

static std::string NormalizePlantName(const std::string& str) {
    std::string clean = "";
    for (char c : str) {
        if (c != ' ' && c != '-' && c != '_') {
            clean += (char)std::tolower(c);
        }
    }
    return clean;
}

ProfileManager& ProfileManager::GetInstance() {
    static ProfileManager instance;
    return instance;
}

std::vector<std::string> ProfileManager::GetAllPlantTypes() {
    return {
        "PeaShooter", "SunFlower", "CherryBomb", "Wallnut", "PotatoMine",
        "SnowPea", "Chomper", "Repeater", "IceShroom", "Gravebuster",
        "Squash", "FirePea", "GatlingPea", "Torchwood", "Caltrop",
        "SpikeRock", "Garlic", "Cabbagepult", "Cornpult", "Melonpult",
        "Jalapeno", "BowlingNut"
    };
}

std::vector<std::string> ProfileManager::GetDefaultStarterPlants() {
    // All plants in project that are NOT sold in Crazy Dave's shop:
    return {
        "PeaShooter", "SunFlower", "Wallnut", "SnowPea", "Chomper",
        "Repeater", "FirePea", "Gravebuster", "Caltrop", "BowlingNut"
    };
}

std::string ProfileManager::getProfileFilePath(const std::string& name) const {
    return "userdata/" + name + ".json";
}

bool ProfileManager::loadProfileFromFile(const std::string& filePath, UserProfile& outProfile) const {
    if (!fs::exists(filePath)) return false;
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) return false;

        json j;
        file >> j;

        outProfile.name = j.value("name", "Unknown");
        outProfile.coins = j.value("coins", 1000);
        outProfile.maxLevel = j.value("maxLevel", 1);
        outProfile.unlockedPlants.clear();

        if (j.contains("unlockedPlants") && j["unlockedPlants"].is_array()) {
            for (const auto& item : j["unlockedPlants"]) {
                if (item.is_string()) {
                    outProfile.unlockedPlants.push_back(item.get<std::string>());
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading profile from " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

void ProfileManager::saveProfileToFile(const UserProfile& profile) const {
    try {
        fs::create_directories("userdata");
        std::string path = getProfileFilePath(profile.name);
        std::ofstream file(path);
        if (file.is_open()) {
            json j;
            j["name"] = profile.name;
            j["coins"] = profile.coins;
            j["maxLevel"] = profile.maxLevel;
            j["unlockedPlants"] = profile.unlockedPlants;
            file << j.dump(4);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving profile " << profile.name << ": " << e.what() << std::endl;
    }
}

void ProfileManager::saveActiveUserConfig() {
    try {
        fs::create_directories("userdata");
        std::ofstream file("userdata/config.json");
        if (file.is_open()) {
            json j;
            j["activeUser"] = m_activeUserName;
            file << j.dump(4);
        }
    } catch (...) {}
}

void ProfileManager::loadActiveUserConfig() {
    if (fs::exists("userdata/config.json")) {
        try {
            std::ifstream file("userdata/config.json");
            if (file.is_open()) {
                json j;
                file >> j;
                m_activeUserName = j.value("activeUser", "admin");
            }
        } catch (...) {
            m_activeUserName = "admin";
        }
    } else {
        m_activeUserName = "admin";
    }
}

void ProfileManager::Init() {
    fs::create_directories("userdata");
    m_profiles.clear();

    // 1. Scan existing user profile files in userdata/
    for (const auto& entry : fs::directory_iterator("userdata")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string filename = entry.path().stem().string();
            if (filename == "config") continue;

            UserProfile prof;
            if (loadProfileFromFile(entry.path().string(), prof)) {
                m_profiles.push_back(prof);
            }
        }
    }

    // 2. Ensure default "admin" account exists with 500,000 coins and full plants
    bool hasAdmin = false;
    for (const auto& p : m_profiles) {
        if (ToLowerStr(p.name) == "admin") {
            hasAdmin = true;
            break;
        }
    }

    if (!hasAdmin) {
        UserProfile adminProf;
        adminProf.name = "admin";
        adminProf.coins = 500000;
        adminProf.unlockedPlants = GetAllPlantTypes();
        m_profiles.push_back(adminProf);
        saveProfileToFile(adminProf);
    }

    // 3. Load active user name from config
    loadActiveUserConfig();

    // 4. Set active profile
    if (!SetActiveUser(m_activeUserName)) {
        SetActiveUser("admin");
    }
}

std::vector<std::string> ProfileManager::GetAllUserNames() const {
    std::vector<std::string> list;
    for (const auto& p : m_profiles) {
        list.push_back(p.name);
    }
    return list;
}

UserProfile ProfileManager::GetActiveProfile() const {
    return m_activeProfile;
}

std::string ProfileManager::GetActiveUserName() const {
    return m_activeUserName;
}

bool ProfileManager::GetProfile(const std::string& name, UserProfile& outProfile) const {
    for (const auto& p : m_profiles) {
        if (p.name == name) {
            outProfile = p;
            return true;
        }
    }
    return false;
}

bool ProfileManager::SetActiveUser(const std::string& name) {
    for (const auto& p : m_profiles) {
        if (p.name == name) {
            m_activeUserName = p.name;
            m_activeProfile = p;
            saveActiveUserConfig();
            return true;
        }
    }
    return false;
}

bool ProfileManager::CreateUser(const std::string& name, std::string& outError) {
    std::string trimmed = name;
    // Trim whitespace
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);

    if (trimmed.empty()) {
        outError = "Name cannot be empty!";
        return false;
    }

    if (trimmed.length() > 14) {
        outError = "Name too long (max 14 chars)!";
        return false;
    }

    // Check invalid file characters
    std::string invalidChars = "\\/:*?\"<>|";
    for (char c : trimmed) {
        if (invalidChars.find(c) != std::string::npos) {
            outError = "Name contains invalid characters!";
            return false;
        }
    }

    // Check case-insensitive duplicate
    std::string lowerNew = ToLowerStr(trimmed);
    for (const auto& p : m_profiles) {
        if (ToLowerStr(p.name) == lowerNew) {
            outError = "User name already exists!";
            return false;
        }
    }

    // Create new profile with starter plants ($1000 coins + non-shop project plants)
    UserProfile newProf;
    newProf.name = trimmed;
    newProf.coins = 1000;
    newProf.unlockedPlants = GetDefaultStarterPlants();

    m_profiles.push_back(newProf);
    saveProfileToFile(newProf);

    // Set as active user
    SetActiveUser(newProf.name);
    return true;
}

bool ProfileManager::DeleteUser(const std::string& name) {
    if (ToLowerStr(name) == "admin") {
        return false; // Cannot delete admin
    }

    auto it = std::find_if(m_profiles.begin(), m_profiles.end(), [&](const UserProfile& p) {
        return p.name == name;
    });

    if (it != m_profiles.end()) {
        std::string filePath = getProfileFilePath(name);
        try {
            if (fs::exists(filePath)) {
                fs::remove(filePath);
            }
        } catch (...) {}

        m_profiles.erase(it);

        if (m_activeUserName == name) {
            SetActiveUser("admin");
        }
        return true;
    }
    return false;
}

void ProfileManager::SaveCurrentProfile() {
    for (auto& p : m_profiles) {
        if (p.name == m_activeUserName) {
            p = m_activeProfile;
            saveProfileToFile(p);
            break;
        }
    }
}

void ProfileManager::SaveProfile(const UserProfile& profile) {
    for (auto& p : m_profiles) {
        if (p.name == profile.name) {
            p = profile;
            saveProfileToFile(p);
            if (m_activeUserName == profile.name) {
                m_activeProfile = profile;
            }
            break;
        }
    }
}

void ProfileManager::SetActiveCoins(int coins) {
    m_activeProfile.coins = coins;
    SaveCurrentProfile();
}

void ProfileManager::AddCoins(int amount) {
    m_activeProfile.coins += amount;
    SaveCurrentProfile();
}

void ProfileManager::UnlockPlant(const std::string& plantName) {
    if (!IsPlantUnlocked(plantName)) {
        m_activeProfile.unlockedPlants.push_back(plantName);
        SaveCurrentProfile();
    }
}

bool ProfileManager::IsPlantUnlocked(const std::string& plantName) const {
    std::string lowerTarget = NormalizePlantName(plantName);
    for (const auto& p : m_activeProfile.unlockedPlants) {
        if (NormalizePlantName(p) == lowerTarget) {
            return true;
        }
    }
    return false;
}

void ProfileManager::SetMaxLevel(int level) {
    m_activeProfile.maxLevel = std::max(1, level);
    SaveCurrentProfile();
}

void ProfileManager::UnlockNextLevel(int completedLevel) {
    if (completedLevel >= m_activeProfile.maxLevel) {
        m_activeProfile.maxLevel = completedLevel + 1;
        SaveCurrentProfile();
    }
}
