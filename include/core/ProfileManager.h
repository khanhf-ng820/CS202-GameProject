#pragma once
#include <string>
#include <vector>

struct UserProfile {
    std::string name;
    int coins = 1000;
    std::vector<std::string> unlockedPlants;
};

class ProfileManager {
public:
    static ProfileManager& GetInstance();

    void Init();

    std::vector<std::string> GetAllUserNames() const;
    UserProfile GetActiveProfile() const;
    std::string GetActiveUserName() const;
    bool GetProfile(const std::string& name, UserProfile& outProfile) const;

    bool SetActiveUser(const std::string& name);
    bool CreateUser(const std::string& name, std::string& outError);
    bool DeleteUser(const std::string& name);

    void SaveCurrentProfile();
    void SaveProfile(const UserProfile& profile);
    void SetActiveCoins(int coins);
    void AddCoins(int amount);
    void UnlockPlant(const std::string& plantName);
    bool IsPlantUnlocked(const std::string& plantName) const;

    static std::vector<std::string> GetAllPlantTypes();
    static std::vector<std::string> GetDefaultStarterPlants();

private:
    ProfileManager() = default;
    ~ProfileManager() = default;
    ProfileManager(const ProfileManager&) = delete;
    ProfileManager& operator=(const ProfileManager&) = delete;

    std::string getProfileFilePath(const std::string& name) const;
    bool loadProfileFromFile(const std::string& filePath, UserProfile& outProfile) const;
    void saveProfileToFile(const UserProfile& profile) const;
    void saveActiveUserConfig();
    void loadActiveUserConfig();

    std::vector<UserProfile> m_profiles;
    std::string m_activeUserName = "admin";
    UserProfile m_activeProfile;
};
