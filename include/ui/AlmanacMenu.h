#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "BitmapFont.h"
#include <string>
#include <vector>

enum class AlmanacPage {
    Index,
    Plants,
    Zombies
};

enum class AlmanacGroundType {
    Day,
    Night,
    Pool,
    NightPool,
    Roof,
    Ice
};

struct PlantAlmanacData {
    std::string name;
    std::string packetKey;
    std::string reanimPath;
    std::string defaultAnim;
    AlmanacGroundType ground;
    int sunCost;
    std::string recharge;
    std::string damage;
    std::string description;
    std::string flavor;
    float animScale;
    float previewOffsetX;
    float previewOffsetY;
    Color tint;
};

struct ZombieAlmanacData {
    std::string name;
    std::string reanimPath;
    std::string defaultAnim;
    AlmanacGroundType ground;
    std::string toughness;
    std::string speed;
    std::string description;
    std::string flavor;
    float animScale;
    float previewOffsetX;
    float previewOffsetY;
    float iconScale;
    float iconOffsetX;
    float iconOffsetY;
};

class AlmanacMenu {
public:
    AlmanacMenu(Resources& res);
    ~AlmanacMenu() = default;

    void update(float dt, bool& showAlmanac);
    void draw();

private:
    void initData();
    void loadSelectedPlantPreview();
    void loadSelectedZombiePreview();

    bool isButtonHovered(Vector2 mousePos, Rectangle bounds);
    void drawButton(Rectangle bounds, const char* text, Texture2D normalTex, Texture2D hoverTex, bool isHovered);
    void drawWrappedText(const BitmapFont& font, const std::string& text, float startX, float startY, float maxWidth, float lineHeight, float scale, Color tint);

    Resources& m_res;
    AlmanacPage m_currentPage;

    // Fonts
    BitmapFont m_titleFont;       // HouseofTerror28
    BitmapFont m_headerFont;      // DwarvenTodcraft24
    BitmapFont m_subFont;         // DwarvenTodcraft18
    BitmapFont m_bodyFont;        // BrianneTod16
    BitmapFont m_numFont;         // ContinuumBold14

    // Common Textures
    Texture2D m_indexBack;
    Texture2D m_plantBack;
    Texture2D m_zombieBack;
    Texture2D m_indexBtn;
    Texture2D m_indexBtnHl;
    Texture2D m_closeBtn;
    Texture2D m_closeBtnHl;
    Texture2D m_plantCard;
    Texture2D m_zombieCard;
    Texture2D m_zombieWindow;
    Texture2D m_zombieWindow2;
    Texture2D m_zombieBlank;
    Texture2D m_groundDay;
    Texture2D m_groundNight;
    Texture2D m_groundPool;
    Texture2D m_groundRoof;

    // Data lists
    std::vector<PlantAlmanacData> m_plants;
    std::vector<ZombieAlmanacData> m_zombies;

    int m_selectedPlant;
    int m_selectedZombie;

    // Active Reanimations
    Reanimation m_previewAnim;
    Reanimation m_previewFlagAnim;
    Reanimation m_iconFlagAnim;
    Reanimation m_indexPlantAnim;
    Reanimation m_indexZombieAnim;
    std::vector<Reanimation> m_zombieIconAnims;
};
