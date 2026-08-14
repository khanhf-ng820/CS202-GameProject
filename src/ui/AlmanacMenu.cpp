#include "AlmanacMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <iostream>
#include <algorithm>

AlmanacMenu::AlmanacMenu(Resources& res)
    : m_res(res), m_currentPage(AlmanacPage::Index), m_selectedPlant(0), m_selectedZombie(0) {
    
    // Load Almanac backgrounds and UI textures
    m_indexBack      = res.GetTexture("ALMANAC_INDEXBACK");
    m_plantBack      = res.GetTexture("ALMANAC_PLANTBACK");
    m_zombieBack     = res.GetTexture("ALMANAC_ZOMBIEBACK");
    m_indexBtn       = res.GetTexture("ALMANAC_INDEXBUTTON");
    m_indexBtnHl     = res.GetTexture("ALMANAC_INDEXBUTTONHIGHLIGHT");
    m_closeBtn       = res.GetTexture("ALMANAC_CLOSEBUTTON");
    m_closeBtnHl     = res.GetTexture("ALMANAC_CLOSEBUTTONHIGHLIGHT");
    m_plantCard      = res.GetTexture("ALMANAC_PLANTCARD");
    m_zombieCard     = res.GetTexture("ALMANAC_ZOMBIECARD");
    m_zombieWindow   = res.GetTexture("ALMANAC_ZOMBIEWINDOW");
    m_zombieWindowHl = res.GetTexture("ALMANAC_ZOMBIEWINDOW2");
    m_zombieBlank    = res.GetTexture("ALMANAC_ZOMBIEBLANK");

    // Ground previews
    m_groundDay      = res.GetTexture("ALMANAC_GROUNDDAY");
    m_groundNight    = res.GetTexture("ALMANAC_GROUNDNIGHT");
    m_groundPool     = res.GetTexture("ALMANAC_GROUNDPOOL");
    m_groundRoof     = res.GetTexture("ALMANAC_GROUNDROOF");

    // Zombie Head textures for portraits
    m_zombieHead     = res.GetTexture("ZOMBIE_HEAD");
    m_zombieCone     = res.GetTexture("ZOMBIE_CONE");
    m_zombieBucket   = res.GetTexture("ZOMBIE_BUCKET");
    m_zombieFlag     = res.GetTexture("ZOMBIE_FLAG");
    m_footballHead   = res.GetTexture("ZOMBIE_FOOTBALL_HEAD");
    m_newspaperHead  = res.GetTexture("ZOMBIE_PAPER_HEAD_LOOK");

    // Load Fonts
    m_titleFont.Load(res.GetAssetPath("assets/data/HouseofTerror28.png"), res.GetAssetPath("assets/data/HouseofTerror28.txt"));
    m_headerFont.Load(res.GetAssetPath("assets/data/DwarvenTodcraft24.png"), res.GetAssetPath("assets/data/DwarvenTodcraft24.txt"));
    m_subFont.Load(res.GetAssetPath("assets/data/DwarvenTodcraft18.png"), res.GetAssetPath("assets/data/DwarvenTodcraft18.txt"));
    m_bodyFont.Load(res.GetAssetPath("assets/data/_BrianneTod16.png"), res.GetAssetPath("assets/data/BrianneTod16.txt"));
    m_numFont.Load(res.GetAssetPath("assets/data/_ContinuumBold14.png"), res.GetAssetPath("assets/data/ContinuumBold14.txt"));

    initData();

    // Setup Index screen preview models
    std::string sunPath = res.GetAssetPath("assets/reanim/SunFlower.reanim");
    if (FileExists(sunPath.c_str())) {
        ReanimDefinition sunDef = res.LoadReanim(sunPath);
        m_indexPlantAnim.SetResources(sunDef, res);
        m_indexPlantAnim.SetAnimation("anim_idle");
    }

    std::string zPath = res.GetAssetPath("assets/reanim/Zombie.reanim");
    if (FileExists(zPath.c_str())) {
        ReanimDefinition zDef = res.LoadReanim(zPath);
        m_indexZombieAnim.SetResources(zDef, res);
        m_indexZombieAnim.SetAnimation("anim_walk");
        m_indexZombieAnim.SetTrackVisible("Zombie_cone", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_bucket", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_flag", false);
    }

    loadSelectedPlantPreview();
    loadSelectedZombiePreview();
}

void AlmanacMenu::initData() {
    // 13 Implemented Plants
    m_plants = {
        { "Peashooter",   "PEASHOOTER",   "assets/reanim/PeaShooter.reanim",  "anim_head_idle", AlmanacGroundType::Day,  100, "Fast",      "Normal",         "Shoots peas at attacking zombies.",                                                               "How can a single plant grow and shoot so many peas so quickly?", 1.30f, 550.0f, 88.0f, WHITE },
        { "Sunflower",    "SUNFLOWER",    "assets/reanim/SunFlower.reanim",   "anim_idle",      AlmanacGroundType::Day,  50,  "Fast",      "None",           "Gives you additional sun to plant more plants.",                                                  "Sunflower can't help but dance to the beat of the music.", 1.30f, 552.0f, 90.0f, WHITE },
        { "Cherry Bomb",  "CHERRYBOMB",   "assets/reanim/CherryBomb.reanim",  "anim_idle",      AlmanacGroundType::Day,  150, "Very Slow", "Massive",        "Blows up all zombies in an area.",                                                                "'We want to blow up!' say the Cherry Bomb brothers.", 1.30f, 546.0f, 103.0f, WHITE },
        { "Wall-nut",     "WALLNUT",      "assets/reanim/Wallnut.reanim",     "anim_idle",      AlmanacGroundType::Day,  50,  "Slow",      "None",           "Blocks zombies and protects your other plants.",                                                  "Wall-nut has a hard shell and a kind smile.", 1.30f, 545.0f, 99.0f, WHITE },
        { "Snow Pea",     "SNOWPEA",      "assets/reanim/SnowPea.reanim",     "anim_head_idle", AlmanacGroundType::Day,  175, "Fast",      "Normal (slows)", "Shoots frozen peas that damage and slow down zombies.",                                          "Folks tell Snow Pea how cool he is, and he just blushes.", 1.30f, 549.0f, 90.0f, WHITE },
        { "Chomper",      "CHOMPER",      "assets/reanim/Chomper.reanim",     "anim_idle",      AlmanacGroundType::Day,  150, "Fast",      "Massive",        "Devours a zombie whole, but is vulnerable while chewing.",                                        "Chomper almost got a gig in Hollywood once, but his agent dropped him.", 1.15f, 560.0f, 105.0f, WHITE },
        { "Repeater",     "REPEATER",     "assets/reanim/PeaShooter.reanim",  "anim_head_idle", AlmanacGroundType::Day,  200, "Fast",      "2x Normal",      "Fires two peas at a time.",                                                                       "Repeater is fiercely motivated to defend your lawn.", 1.30f, 550.0f, 88.0f, WHITE },
        { "Jalapeno",     "JALAPENO",     "assets/reanim/Jalapeno.reanim",    "anim_idle",      AlmanacGroundType::Day,  125, "Very Slow", "Massive",        "Destroys an entire lane of zombies in a searing fiery blast.",                                    "'NNNNGGG!' says Jalapeno. He's not going to explode yet, but soon.", 1.30f, 548.0f, 97.0f, WHITE },
        { "Torchwood",    "TORCHWOOD",    "assets/reanim/Torchwood.reanim",   "anim_idle",      AlmanacGroundType::Day,  175, "Fast",      "None",           "Turns peas that pass through it into flaming fireballs.",                                         "Everybody likes Torchwood for his radiant personality.", 1.25f, 549.0f, 107.0f, WHITE },
        { "Cornpult",     "CORNPULT",     "assets/reanim/Cornpult.reanim",    "anim_idle",      AlmanacGroundType::Roof, 100, "Fast",      "Normal / Stun",  "Lobs kernels and butter that temporarily immobilizes zombies.",                                   "Cornpult is the oldest of the Pult siblings.", 1.20f, 565.0f, 116.0f, WHITE },
        { "Melonpult",    "MELONPULT",    "assets/reanim/Melonpult.reanim",   "anim_idle",      AlmanacGroundType::Roof, 300, "Fast",      "Heavy",          "Lobs heavy melons that do massive splash damage across multiple lanes.",                          "Melonpult does not mess around when it comes to defending the roof.", 1.20f, 581.0f, 114.0f, WHITE },
        { "Gatling Pea",  "GATLINGPEA",   "assets/reanim/GatlingPea.reanim",  "anim_head_idle", AlmanacGroundType::Day,  250, "Very Slow", "4x Normal",      "Shoots four peas at a rapid firing rate.",                                                        "Gatling Pea's parents were worried when he decided to join the army.", 1.30f, 542.0f, 96.0f, WHITE },
        { "Fire Pea",     "FIREPEA",      "assets/reanim/PeaShooter.reanim",  "anim_head_idle", AlmanacGroundType::Day,  175, "Fast",      "2x Normal",      "Shoots blazing fireballs that ignite oncoming targets.",                                          "Fire Pea is always ready to bring the heat to the front lines.", 1.30f, 550.0f, 88.0f, Color{ 255, 140, 60, 255 } }
    };

    // 6 Implemented Zombies
    m_zombies = {
        { "Regular Zombie",   "assets/reanim/Zombie.reanim",          "anim_walk", "Low",       "Basic",                    "Regular garden-variety zombie.",                                                                   "This zombie loves brains. Can't get enough of them.", 1.00f, 561.0f, 127.0f },
        { "Flag Zombie",      "assets/reanim/Zombie.reanim",          "anim_walk", "Low",       "Basic",                    "Marks the arrival of a huge wave of zombies.",                                                     "Make no mistake, Flag Zombie loves brains just as much as the next guy.", 1.00f, 567.0f, 127.0f },
        { "Conehead Zombie",  "assets/reanim/Zombie.reanim",          "anim_walk", "Medium",    "Basic",                    "His traffic cone makes him twice as tough as a regular zombie.",                                   "Conehead Zombie was minding his own business when an emergency traffic cone landed on him.", 1.00f, 561.0f, 138.0f },
        { "Buckethead Zombie","assets/reanim/Zombie.reanim",          "anim_walk", "High",      "Basic",                    "His bucket head makes him extremely durable and resistant to damage.",                             "Buckethead Zombie always wore a bucket. In a world of zombies, it made him feel unique.", 1.00f, 563.0f, 134.0f },
        { "Football Zombie",  "assets/reanim/Zombie_football.reanim", "anim_walk", "Very High", "Fast",                     "Heavy helmet and rapid sprint make him a devastating threat on the lawn.",                         "Football Zombie gives 110 percent on the field, though he doesn't know what a football is.", 0.90f, 560.0f, 130.0f },
        { "Newspaper Zombie", "assets/reanim/Zombie_paper.reanim",    "anim_walk", "Medium",    "Basic (Fast when angry)",  "His newspaper protects him until it's destroyed, sending him into an angry sprint.",               "Newspaper Zombie was this close to finishing his Sudoku puzzle.", 0.95f, 563.0f, 125.0f }
    };
}

void AlmanacMenu::loadSelectedPlantPreview() {
    if (m_selectedPlant >= 0 && m_selectedPlant < (int)m_plants.size()) {
        const auto& data = m_plants[m_selectedPlant];
        std::string path = m_res.GetAssetPath(data.reanimPath);
        if (FileExists(path.c_str())) {
            m_previewAnim = Reanimation();
            ReanimDefinition def = m_res.LoadReanim(path);
            m_previewAnim.SetResources(def, m_res);
            m_previewAnim.SetBaseAnimation("anim_idle");

            if (data.name == "Peashooter") {
                m_previewAnim.SetAnimation("anim_head_idle");
                m_previewAnim.SetTrackVisible("idle_headleaf_farthest", false);
                m_previewAnim.SetTrackVisible("idle_headleaf_3rdfarthest", false);
                m_previewAnim.SetTrackVisible("idle_headleaf_nearest", false);
                m_previewAnim.SetTrackVisible("idle_headleaf_tip_top", false);
                m_previewAnim.SetTrackVisible("PeaShooter_eyebrow", false);
            } else if (data.name == "Snow Pea" || data.name == "Repeater" || data.name == "Fire Pea" || data.name == "Gatling Pea") {
                m_previewAnim.SetAnimation("anim_head_idle");
            } else if (data.name == "Cornpult") {
                m_previewAnim.SetAnimation("anim_idle");
                m_previewAnim.SetTrackVisible("Cornpult_kernal", true);
                m_previewAnim.SetTrackVisible("Cornpult_butter", false);
            } else if (data.name == "Melonpult") {
                m_previewAnim.SetAnimation("anim_idle");
                m_previewAnim.SetTrackVisible("Melonpult_mellon", true);
            } else {
                m_previewAnim.SetAnimation(data.defaultAnim);
            }
        }
    }
}

void AlmanacMenu::loadSelectedZombiePreview() {
    if (m_selectedZombie >= 0 && m_selectedZombie < (int)m_zombies.size()) {
        const auto& data = m_zombies[m_selectedZombie];
        std::string path = m_res.GetAssetPath(data.reanimPath);
        if (FileExists(path.c_str())) {
            m_previewAnim = Reanimation();
            ReanimDefinition def = m_res.LoadReanim(path);
            m_previewAnim.SetResources(def, m_res);
            m_previewAnim.SetBaseAnimation(data.defaultAnim);
            m_previewAnim.SetAnimation(data.defaultAnim);

            // Configure armored zombie accessories
            if (data.name == "Regular Zombie") {
                m_previewAnim.SetTrackVisible("anim_bucket", false);
                m_previewAnim.SetTrackVisible("anim_cone", false);
                m_previewAnim.SetTrackVisible("anim_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_duckytube", false);
                m_previewAnim.SetTrackVisible("Zombie_mustache", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_flaghand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (data.name == "Flag Zombie") {
                m_previewAnim.SetTrackVisible("anim_bucket", false);
                m_previewAnim.SetTrackVisible("anim_cone", false);
                m_previewAnim.SetTrackVisible("anim_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_duckytube", false);
                m_previewAnim.SetTrackVisible("Zombie_mustache", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_flaghand", true);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_hand", false);
            } else if (data.name == "Conehead Zombie") {
                m_previewAnim.SetTrackVisible("anim_bucket", false);
                m_previewAnim.SetTrackVisible("anim_cone", true);
                m_previewAnim.SetTrackVisible("anim_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_duckytube", false);
                m_previewAnim.SetTrackVisible("Zombie_mustache", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_flaghand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (data.name == "Buckethead Zombie") {
                m_previewAnim.SetTrackVisible("anim_bucket", true);
                m_previewAnim.SetTrackVisible("anim_cone", false);
                m_previewAnim.SetTrackVisible("anim_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_duckytube", false);
                m_previewAnim.SetTrackVisible("Zombie_mustache", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_flaghand", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (data.name == "Football Zombie") {
                m_previewAnim.SetTrackVisible("zombie_football_helmet", true);
                m_previewAnim.BakeChildRotation("zombie_football_rightleg_lower", "zombie_football_rightleg_foot", "anim_eat");
                m_previewAnim.BakeChildRotation("zombie_football_leftleg_lower", "zombie_football_leftleg_foot", "anim_eat");
            } else if (data.name == "Newspaper Zombie") {
                m_previewAnim.SetTrackVisible("Zombie_paper_paper", true);
            }
        }
    }
}

bool AlmanacMenu::isButtonHovered(Vector2 mousePos, Rectangle bounds) {
    return CheckCollisionPointRec(mousePos, bounds);
}

void AlmanacMenu::drawButton(Rectangle bounds, const char* text, Texture2D normalTex, Texture2D hoverTex, bool isHovered) {
    Texture2D tex = (isHovered && hoverTex.id != 0) ? hoverTex : normalTex;
    if (tex.id != 0) {
        DrawTexturePro(
            tex,
            { 0.0f, 0.0f, (float)tex.width, (float)tex.height },
            bounds,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(bounds, isHovered ? DARKGREEN : DARKGRAY);
        DrawRectangleLinesEx(bounds, 2.0f, GOLD);
    }

    if (text && text[0] != '\0') {
        m_subFont.DrawTextCentered(text, bounds, 0.72f, isHovered ? GREEN : Color{ 230, 210, 160, 255 });
    }
}

void AlmanacMenu::drawWrappedText(const BitmapFont& font, const std::string& text, float startX, float startY, float maxWidth, float lineHeight, float scale, Color tint) {
    std::string currentLine = "";
    float curY = startY;
    size_t start = 0;

    while (start < text.length()) {
        size_t space = text.find(' ', start);
        std::string word = (space == std::string::npos) ? text.substr(start) : text.substr(start, space - start);
        std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);

        if (font.MeasureText(testLine.c_str(), scale) > maxWidth && !currentLine.empty()) {
            font.DrawText(currentLine.c_str(), startX, curY, scale, tint);
            curY += lineHeight;
            currentLine = word;
        } else {
            currentLine = testLine;
        }

        if (space == std::string::npos) break;
        start = space + 1;
    }

    if (!currentLine.empty()) {
        font.DrawText(currentLine.c_str(), startX, curY, scale, tint);
    }
}

void AlmanacMenu::update(float dt, bool& showAlmanac) {
    Vector2 mousePos = GetVirtualMousePosition();
    std::string sfxClick = m_res.GetAssetPath("assets/sounds/gravebutton.ogg");
    std::string sfxPaper = m_res.GetAssetPath("assets/sounds/paper.ogg");

    if (m_currentPage == AlmanacPage::Index) {
        m_indexPlantAnim.Update(dt);
        m_indexZombieAnim.Update(dt);

        if (IsKeyPressed(KEY_ESCAPE)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxClick);
            showAlmanac = false;
            return;
        }

        // View Plants button (164x26)
        Rectangle plantsBtnRect = { 130.0f, 365.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, plantsBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Plants;
            loadSelectedPlantPreview();
            return;
        }

        // View Zombies button (164x26)
        Rectangle zombiesBtnRect = { 505.0f, 365.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, zombiesBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Zombies;
            loadSelectedZombiePreview();
            return;
        }

        // Close button (89x26)
        Rectangle closeBtnRect = { 660.0f, 535.0f, 89.0f, 26.0f };
        if (isButtonHovered(mousePos, closeBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxClick);
            showAlmanac = false;
            return;
        }
    } else if (m_currentPage == AlmanacPage::Plants) {
        m_previewAnim.Update(dt);

        if (IsKeyPressed(KEY_ESCAPE)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        // Check grid clicks (8 cols x 6 rows)
        float startX = 26.0f;
        float startY = 93.0f;
        float cardW  = 50.0f;
        float cardH  = 70.0f;
        int cols     = 8;

        for (int i = 0; i < (int)m_plants.size(); ++i) {
            int row = i / cols;
            int col = i % cols;
            Rectangle cardRect = { startX + col * 52.0f, startY + row * 78.0f, cardW, cardH };

            if (CheckCollisionPointRec(mousePos, cardRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (m_selectedPlant != i) {
                    m_selectedPlant = i;
                    AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
                    loadSelectedPlantPreview();
                }
            }
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 538.0f, 120.0f, 26.0f };
        if (isButtonHovered(mousePos, indexBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        Rectangle switchBtnRect = { 350.0f, 538.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, switchBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Zombies;
            loadSelectedZombiePreview();
            return;
        }

        Rectangle closeBtnRect = { 660.0f, 538.0f, 89.0f, 26.0f };
        if (isButtonHovered(mousePos, closeBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxClick);
            showAlmanac = false;
            return;
        }
    } else if (m_currentPage == AlmanacPage::Zombies) {
        m_previewAnim.Update(dt);

        if (IsKeyPressed(KEY_ESCAPE)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        // Check zombie grid clicks (5 cols x 6 rows)
        float startX = 35.0f;
        float startY = 80.0f;
        float slotW  = 64.0f;
        float slotH  = 64.0f;
        int cols     = 5;

        for (int i = 0; i < (int)m_zombies.size(); ++i) {
            int row = i / cols;
            int col = i % cols;
            Rectangle slotRect = { startX + col * 68.0f, startY + row * 68.0f, slotW, slotH };

            if (CheckCollisionPointRec(mousePos, slotRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (m_selectedZombie != i) {
                    m_selectedZombie = i;
                    AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
                    loadSelectedZombiePreview();
                }
            }
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 538.0f, 120.0f, 26.0f };
        if (isButtonHovered(mousePos, indexBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        Rectangle switchBtnRect = { 350.0f, 538.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, switchBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Plants;
            loadSelectedPlantPreview();
            return;
        }

        Rectangle closeBtnRect = { 660.0f, 538.0f, 89.0f, 26.0f };
        if (isButtonHovered(mousePos, closeBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxClick);
            showAlmanac = false;
            return;
        }
    }
}

void AlmanacMenu::draw() {
    Vector2 mousePos = GetVirtualMousePosition();

    if (m_currentPage == AlmanacPage::Index) {
        // Draw Index Background
        if (m_indexBack.id != 0) {
            DrawTexture(m_indexBack, 0, 0, WHITE);
        } else {
            DrawRectangle(0, 0, 800, 600, DARKBLUE);
        }

        // Animated Mini Models
        m_indexPlantAnim.Draw(200, 260, 1.15f);
        m_indexZombieAnim.Draw(570, 270, 0.85f);

        // Title
        m_titleFont.DrawTextCentered("SUBURBAN ALMANAC", { 0.0f, 32.0f, 800.0f, 40.0f }, 1.3f, Color{ 240, 200, 80, 255 });

        // Buttons
        Rectangle plantsBtnRect = { 130.0f, 365.0f, 164.0f, 26.0f };
        bool plantsHover = isButtonHovered(mousePos, plantsBtnRect);
        drawButton(plantsBtnRect, "VIEW PLANTS", m_indexBtn, m_indexBtnHl, plantsHover);

        Rectangle zombiesBtnRect = { 505.0f, 365.0f, 164.0f, 26.0f };
        bool zombiesHover = isButtonHovered(mousePos, zombiesBtnRect);
        drawButton(zombiesBtnRect, "VIEW ZOMBIES", m_indexBtn, m_indexBtnHl, zombiesHover);

        Rectangle closeBtnRect = { 660.0f, 535.0f, 89.0f, 26.0f };
        bool closeHover = isButtonHovered(mousePos, closeBtnRect);
        drawButton(closeBtnRect, "CLOSE", m_closeBtn, m_closeBtnHl, closeHover);

    } else if (m_currentPage == AlmanacPage::Plants) {
        // Draw Plant Almanac Open Book Background
        if (m_plantBack.id != 0) {
            DrawTexture(m_plantBack, 0, 0, WHITE);
        } else {
            DrawRectangle(0, 0, 800, 600, DARKGREEN);
        }

        // Title
        m_titleFont.DrawTextCentered("PLANTS", { 35.0f, 20.0f, 340.0f, 40.0f }, 1.2f, Color{ 230, 200, 80, 255 });

        // Left Grid of Seed Packets (8 cols x 6 rows = 48 slots matching dashed borders)
        float startX = 26.0f;
        float startY = 93.0f;
        float cardW  = 50.0f;
        float cardH  = 70.0f;
        int cols     = 8;
        int totalSlots = 48;

        for (int i = 0; i < totalSlots; ++i) {
            int row = i / cols;
            int col = i % cols;
            Rectangle cardRect = { startX + col * 52.0f, startY + row * 78.0f, cardW, cardH };

            if (i < (int)m_plants.size()) {
                Texture2D tex = m_res.GetTexture(m_plants[i].packetKey);
                if (tex.id != 0) {
                    DrawTexturePro(
                        tex,
                        { 0.0f, 0.0f, (float)tex.width, (float)tex.height },
                        cardRect,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                } else {
                    DrawRectangleRec(cardRect, DARKGRAY);
                    DrawText(m_plants[i].name.c_str(), (int)cardRect.x + 2, (int)cardRect.y + 10, 8, WHITE);
                }

                if (i == m_selectedPlant) {
                    DrawRectangleLinesEx(cardRect, 3.0f, GOLD);
                } else if (CheckCollisionPointRec(mousePos, cardRect)) {
                    DrawRectangleLinesEx(cardRect, 2.0f, WHITE);
                }
            }
        }

        // Right Display Card (Ground Tile -> Animated Plant -> Parchment Frame Overlay -> Text)
        if (m_selectedPlant >= 0 && m_selectedPlant < (int)m_plants.size()) {
            const auto& plant = m_plants[m_selectedPlant];

            // 1. Ground Tile behind plant
            Texture2D groundTex = (plant.ground == AlmanacGroundType::Roof) ? m_groundRoof : m_groundDay;
            if (groundTex.id != 0) {
                DrawTexturePro(
                    groundTex,
                    { 0.0f, 0.0f, (float)groundTex.width, (float)groundTex.height },
                    { 500.0f, 65.0f, 200.0f, 160.0f },
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }

            // 2. Live Animated Plant Preview centered in the upper slot
            m_previewAnim.Draw(plant.previewOffsetX, plant.previewOffsetY, plant.animScale, plant.tint);

            // 3. Parchment Card Frame overlay
            if (m_plantCard.id != 0) {
                DrawTexture(m_plantCard, 442, 56, WHITE);
            }

            // 4. Plant Name
            m_headerFont.DrawTextCentered(plant.name.c_str(), { 450.0f, 235.0f, 305.0f, 28.0f }, 0.95f, Color{ 60, 40, 20, 255 });

            // 5. Stats
            std::string costStr = "Cost: "     + std::to_string(plant.sunCost);
            std::string rchgStr = "Recharge: " + plant.recharge;
            std::string dmgStr  = "Damage: "   + plant.damage;

            m_subFont.DrawText(costStr.c_str(), 465.0f, 270.0f, 0.70f, Color{ 70, 45, 15, 255 });
            m_subFont.DrawText(rchgStr.c_str(), 465.0f, 290.0f, 0.70f, Color{ 70, 45, 15, 255 });
            m_subFont.DrawText(dmgStr.c_str(),  465.0f, 310.0f, 0.70f, Color{ 70, 45, 15, 255 });

            // 6. Description & Flavor Story (Multiline Wrapped)
            drawWrappedText(m_bodyFont, plant.description, 465.0f, 340.0f, 260.0f, 18.0f, 0.75f, Color{ 40, 30, 20, 255 });
            drawWrappedText(m_bodyFont, plant.flavor,      465.0f, 410.0f, 260.0f, 17.0f, 0.68f, Color{ 110, 85, 60, 255 });
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 538.0f, 120.0f, 26.0f };
        bool indexHover = isButtonHovered(mousePos, indexBtnRect);
        drawButton(indexBtnRect, "INDEX", m_indexBtn, m_indexBtnHl, indexHover);

        Rectangle switchBtnRect = { 350.0f, 538.0f, 164.0f, 26.0f };
        bool switchHover = isButtonHovered(mousePos, switchBtnRect);
        drawButton(switchBtnRect, "VIEW ZOMBIES", m_indexBtn, m_indexBtnHl, switchHover);

        Rectangle closeBtnRect = { 660.0f, 538.0f, 89.0f, 26.0f };
        bool closeHover = isButtonHovered(mousePos, closeBtnRect);
        drawButton(closeBtnRect, "CLOSE", m_closeBtn, m_closeBtnHl, closeHover);

    } else if (m_currentPage == AlmanacPage::Zombies) {
        // Draw Zombie Almanac Open Book Background
        if (m_zombieBack.id != 0) {
            DrawTexture(m_zombieBack, 0, 0, WHITE);
        } else {
            DrawRectangle(0, 0, 800, 600, DARKPURPLE);
        }

        // Title
        m_titleFont.DrawTextCentered("ZOMBIES", { 35.0f, 20.0f, 340.0f, 40.0f }, 1.2f, Color{ 230, 200, 80, 255 });

        // Left Grid of Zombie Windows (5 cols x 6 rows = 30 slots)
        float startX = 35.0f;
        float startY = 80.0f;
        float slotW  = 64.0f;
        float slotH  = 64.0f;
        int cols     = 5;
        int totalSlots = 30;

        for (int i = 0; i < totalSlots; ++i) {
            int row = i / cols;
            int col = i % cols;
            Rectangle slotRect = { startX + col * 68.0f, startY + row * 68.0f, slotW, slotH };

            if (i < (int)m_zombies.size()) {
                Texture2D winTex = (i == m_selectedZombie) ? m_zombieWindowHl : m_zombieWindow;
                if (winTex.id != 0) {
                    DrawTexturePro(
                        winTex,
                        { 0.0f, 0.0f, (float)winTex.width, (float)winTex.height },
                        slotRect,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                } else {
                    DrawRectangleRec(slotRect, DARKGRAY);
                }

                // Draw Head Portrait inside window
                Rectangle headRect = { slotRect.x + 10.0f, slotRect.y + 10.0f, 44.0f, 44.0f };
                if (i == 0 || i == 1 || i == 2 || i == 3) {
                    if (m_zombieHead.id != 0) {
                        DrawTexturePro(m_zombieHead, { 0, 0, (float)m_zombieHead.width, (float)m_zombieHead.height }, headRect, { 0, 0 }, 0.0f, WHITE);
                    }
                    if (i == 1 && m_zombieFlag.id != 0) {
                        DrawTexturePro(m_zombieFlag, { 0, 0, (float)m_zombieFlag.width, (float)m_zombieFlag.height }, { headRect.x + 15, headRect.y - 10, 25, 25 }, { 0, 0 }, 0.0f, WHITE);
                    } else if (i == 2 && m_zombieCone.id != 0) {
                        DrawTexturePro(m_zombieCone, { 0, 0, (float)m_zombieCone.width, (float)m_zombieCone.height }, { headRect.x + 8, headRect.y - 12, 28, 28 }, { 0, 0 }, 0.0f, WHITE);
                    } else if (i == 3 && m_zombieBucket.id != 0) {
                        DrawTexturePro(m_zombieBucket, { 0, 0, (float)m_zombieBucket.width, (float)m_zombieBucket.height }, { headRect.x + 6, headRect.y - 10, 30, 28 }, { 0, 0 }, 0.0f, WHITE);
                    }
                } else if (i == 4 && m_footballHead.id != 0) {
                    DrawTexturePro(m_footballHead, { 0, 0, (float)m_footballHead.width, (float)m_footballHead.height }, headRect, { 0, 0 }, 0.0f, WHITE);
                } else if (i == 5 && m_newspaperHead.id != 0) {
                    DrawTexturePro(m_newspaperHead, { 0, 0, (float)m_newspaperHead.width, (float)m_newspaperHead.height }, headRect, { 0, 0 }, 0.0f, WHITE);
                }

                if (i == m_selectedZombie) {
                    DrawRectangleLinesEx(slotRect, 3.0f, GOLD);
                } else if (CheckCollisionPointRec(mousePos, slotRect)) {
                    DrawRectangleLinesEx(slotRect, 2.0f, WHITE);
                }
            } else {
                // Empty blank slot
                if (m_zombieBlank.id != 0) {
                    DrawTexturePro(
                        m_zombieBlank,
                        { 0.0f, 0.0f, (float)m_zombieBlank.width, (float)m_zombieBlank.height },
                        slotRect,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                } else {
                    DrawRectangleRec(slotRect, ColorAlpha(BLACK, 0.3f));
                }
            }
        }

        // Right Display Card (Animated Zombie -> Parchment Frame Overlay -> Text)
        if (m_selectedZombie >= 0 && m_selectedZombie < (int)m_zombies.size()) {
            const auto& zombie = m_zombies[m_selectedZombie];

            // 1. Live Animated Preview centered in the upper slot
            m_previewAnim.Draw(zombie.previewOffsetX, zombie.previewOffsetY, zombie.animScale);

            // 2. Parchment Card Frame overlay
            if (m_zombieCard.id != 0) {
                DrawTexture(m_zombieCard, 442, 50, WHITE);
            }

            // 3. Zombie Name
            m_headerFont.DrawTextCentered(zombie.name.c_str(), { 450.0f, 245.0f, 305.0f, 28.0f }, 0.95f, Color{ 60, 40, 20, 255 });

            // 4. Stats
            std::string tghStr = "Toughness: " + zombie.toughness;
            std::string spdStr = "Speed: "     + zombie.speed;

            m_subFont.DrawText(tghStr.c_str(), 465.0f, 285.0f, 0.70f, Color{ 70, 45, 15, 255 });
            m_subFont.DrawText(spdStr.c_str(), 465.0f, 310.0f, 0.70f, Color{ 70, 45, 15, 255 });

            // 5. Description & Flavor Story (Multiline Wrapped)
            drawWrappedText(m_bodyFont, zombie.description, 465.0f, 345.0f, 260.0f, 18.0f, 0.75f, Color{ 40, 30, 20, 255 });
            drawWrappedText(m_bodyFont, zombie.flavor,      465.0f, 410.0f, 260.0f, 17.0f, 0.68f, Color{ 110, 85, 60, 255 });
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 538.0f, 120.0f, 26.0f };
        bool indexHover = isButtonHovered(mousePos, indexBtnRect);
        drawButton(indexBtnRect, "INDEX", m_indexBtn, m_indexBtnHl, indexHover);

        Rectangle switchBtnRect = { 350.0f, 538.0f, 164.0f, 26.0f };
        bool switchHover = isButtonHovered(mousePos, switchBtnRect);
        drawButton(switchBtnRect, "VIEW PLANTS", m_indexBtn, m_indexBtnHl, switchHover);

        Rectangle closeBtnRect = { 660.0f, 538.0f, 89.0f, 26.0f };
        bool closeHover = isButtonHovered(mousePos, closeBtnRect);
        drawButton(closeBtnRect, "CLOSE", m_closeBtn, m_closeBtnHl, closeHover);
    }
}
