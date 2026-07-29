#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"
#include <vector>
#include <string>
#include <unordered_map>

struct SeedSelectCard {
    std::string plantType;
    int cost;
    std::string textureKey;
    Rectangle bounds;
    bool isChosen;
};

class SeedSelectMenu {
private:
    std::vector<SeedSelectCard> m_availableCards;
    std::vector<std::string> m_chosenPlants; // Max 7 deck size
    std::unordered_map<std::string, int> m_plantCosts;
    
    Rectangle m_bankBounds;
    Rectangle m_chooserBounds;
    Rectangle m_confirmBtnBounds;

    BitmapFont m_font;
    BitmapFont m_houseOfTerrorFont;
    BitmapFont m_sunFont;
    BitmapFont m_priceFont;

    Texture2D m_bankTex;
    Texture2D m_chooserBgTex;
    Texture2D m_confirmBtnTex;
    Texture2D m_confirmBtnGlowTex;
    Texture2D m_confirmBtnDisabledTex;
    Texture2D m_silhouetteTex;

    void initCatalog(Resources& res);

public:
    SeedSelectMenu(Resources& res);
    ~SeedSelectMenu() = default;

    // Returns true when confirm button ("LET'S ROCK!") is clicked to launch gameplay
    bool update(float dt, Vector2 mousePos, bool mouseClicked);
    void draw(Resources& res, Vector2 mousePos) const;

    const std::vector<std::string>& getChosenDeck() const { return m_chosenPlants; }
};
