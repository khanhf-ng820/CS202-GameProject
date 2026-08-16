#pragma once
#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "BitmapFont.h"

enum class MenuAction {
    None,
    StartAdventure,
    Level1,
    Level2,
    Level3,
    Options,
    Help,
    Shop,
    ZenGarden,
    Quit
};

class MainMenu {
public:
    MainMenu(Resources& res);
    ~MainMenu();

    void update(float dt);
    void draw();
    MenuAction getAction() const;
    void resetAction();

private:
    Resources& m_res;
    MenuAction m_action;
    Reanimation m_anim;                   // SelectorScreen.reanim
    std::vector<Reanimation> m_cloudAnims; // Multiple concurrent moving cloud animations
    BitmapFont m_font;                    // DwarvenTodcraft24 bitmap font

    // Manually loaded textures for bottom-bar buttons (not in reanim)
    Texture2D m_optionsBtn;
    Texture2D m_optionsBtnHl;
    Texture2D m_helpBtn;
    Texture2D m_helpBtnHl;
    Texture2D m_quitBtn;
    Texture2D m_quitBtnHl;
    Texture2D m_storeBtn;
    Texture2D m_storeBtnHl;
    Texture2D m_zenGardenBtn;
    Texture2D m_zenGardenBtnHl;
    Texture2D m_bgTex;

    bool isGraveButtonHovered(Vector2 mousePos, Rectangle bounds, const std::string& texName);
};
