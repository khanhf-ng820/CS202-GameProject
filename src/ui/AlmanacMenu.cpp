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
    m_zombieWindow2  = res.GetTexture("ALMANAC_ZOMBIEWINDOW2");
    m_zombieBlank    = res.GetTexture("ALMANAC_ZOMBIEBLANK");

    // Ground previews
    m_groundDay      = res.GetTexture("ALMANAC_GROUNDDAY");
    m_groundNight    = res.GetTexture("ALMANAC_GROUNDNIGHT");
    m_groundPool     = res.GetTexture("ALMANAC_GROUNDPOOL");
    m_groundRoof     = res.GetTexture("ALMANAC_GROUNDROOF");

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
        m_indexZombieAnim.SetTrackVisible("anim_bucket", false);
        m_indexZombieAnim.SetTrackVisible("anim_cone", false);
        m_indexZombieAnim.SetTrackVisible("anim_screendoor", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_duckytube", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_mustache", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_innerarm_screendoor", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_flaghand", false);
        m_indexZombieAnim.SetTrackVisible("Zombie_outerarm_hand", true);
    }

    loadSelectedPlantPreview();
    loadSelectedZombiePreview();
}

void AlmanacMenu::initData() {
    // 19 Implemented Plants
    m_plants = {
        { "Peashooter",   "PEASHOOTER",   "assets/reanim/PeaShooter.reanim",  "anim_head_idle", AlmanacGroundType::Day,  100, "Fast",      "Normal",         "Shoots peas at attacking zombies.",                                                               "How can a single plant grow and shoot so many peas so quickly?", 1.30f, 566.0f, 117.0f, WHITE },
        { "Sunflower",    "SUNFLOWER",    "assets/reanim/SunFlower.reanim",   "anim_idle",      AlmanacGroundType::Day,  50,  "Fast",      "None",           "Gives you additional sun to plant more plants.",                                                  "Sunflower can't help but dance to the beat of the music.", 1.30f, 568.0f, 119.0f, WHITE },
        { "Cherry Bomb",  "CHERRYBOMB",   "assets/reanim/CherryBomb.reanim",  "anim_idle",      AlmanacGroundType::Day,  150, "Very Slow", "Massive",        "Blows up all zombies in an area.",                                                                "'We want to blow up!' say the Cherry Bomb brothers.", 1.30f, 562.0f, 132.0f, WHITE },
        { "Wall-nut",     "WALLNUT",      "assets/reanim/Wallnut.reanim",     "anim_idle",      AlmanacGroundType::Day,  50,  "Slow",      "None",           "Blocks zombies and protects your other plants.",                                                  "Wall-nut has a hard shell and a kind smile.", 1.30f, 561.0f, 128.0f, WHITE },
        { "Potato Mine",  "POTATOMINE",   "assets/reanim/PotatoMine.reanim",  "anim_idle",      AlmanacGroundType::Day,  25,  "Slow",      "Massive",        "Explodes on contact, but takes time to arm itself.",                                              "Spudow!", 1.30f, 565.0f, 130.0f, WHITE },
        { "Snow Pea",     "SNOWPEA",      "assets/reanim/SnowPea.reanim",     "anim_head_idle", AlmanacGroundType::Day,  175, "Fast",      "Normal (slows)", "Shoots frozen peas that damage and slow down zombies.",                                          "Folks tell Snow Pea how cool he is, and he just blushes.", 1.30f, 565.0f, 119.0f, WHITE },
        { "Chomper",      "CHOMPER",      "assets/reanim/Chomper.reanim",     "anim_idle",      AlmanacGroundType::Day,  150, "Fast",      "Massive",        "Devours a zombie whole, but is vulnerable while chewing.",                                        "Chomper almost got a gig in Hollywood once, but his agent dropped him.", 1.15f, 576.0f, 134.0f, WHITE },
        { "Repeater",     "REPEATER",     "assets/reanim/PeaShooter.reanim",  "anim_head_idle", AlmanacGroundType::Day,  200, "Fast",      "2x Normal",      "Fires two peas at a time.",                                                                       "Repeater is fiercely motivated to defend your lawn.", 1.30f, 566.0f, 117.0f, WHITE },
        { "Ice-shroom",   "ICESHROOM",    "assets/reanim/IceShroom.reanim",   "anim_idle",      AlmanacGroundType::Day,  75,  "Very Slow", "None",           "Freezes all zombies on screen temporarily.",                                                      "Ice-shroom frowns, not because he's unhappy, but because of a childhood facial injury.", 1.25f, 565.0f, 130.0f, WHITE },
        { "Grave Buster", "GRAVEBUSTER",  "assets/reanim/Gravebuster.reanim", "anim_idle",      AlmanacGroundType::Day,  75,  "Slow",      "None",           "Land on graves and consume them.",                                                                "Despite Gravebuster's rough appearance, he loves puppies.", 1.25f, 565.0f, 130.0f, WHITE },
        { "Squash",       "SQUASH",       "assets/reanim/Squash.reanim",      "anim_idle",      AlmanacGroundType::Day,  50,  "Slow",      "Massive",        "Squashes the first zombie that gets close to it.",                                                "'I'm ready!' says Squash. 'Let's do it!'", 1.25f, 565.0f, 130.0f, WHITE },
        { "Jalapeno",     "JALAPENO",     "assets/reanim/Jalapeno.reanim",    "anim_idle",      AlmanacGroundType::Day,  125, "Very Slow", "Massive",        "Destroys an entire lane of zombies in a searing fiery blast.",                                    "'NNNNGGG!' says Jalapeno. He's not going to explode yet, but soon.", 1.30f, 564.0f, 126.0f, WHITE },
        { "Spikeweed",    "CALTROP",      "assets/reanim/Caltrop.reanim",     "anim_idle",      AlmanacGroundType::Day,  100, "Fast",      "Normal",         "Hurts any zombies that step on it.",                                                             "Spikeweed loves hockey.", 1.25f, 565.0f, 140.0f, WHITE },
        { "Torchwood",    "TORCHWOOD",    "assets/reanim/Torchwood.reanim",   "anim_idle",      AlmanacGroundType::Day,  175, "Fast",      "None",           "Turns peas that pass through it into flaming fireballs.",                                         "Everybody likes Torchwood for his radiant personality.", 1.25f, 565.0f, 136.0f, WHITE },
        { "Spikerock",    "SPIKEROCK",    "assets/reanim/SpikeRock.reanim",   "anim_idle",      AlmanacGroundType::Day,  125, "Very Slow", "2x Normal",      "Deals heavy damage to walking zombies that step on it.",                                          "Spikerock is hard as nails.", 1.25f, 565.0f, 140.0f, WHITE },
        { "Garlic",       "GARLIC",       "assets/reanim/Garlic.reanim",      "anim_idle",      AlmanacGroundType::Day,  50,  "Fast",      "None",           "Diverts zombies into other lanes when they bite it.",                                             "Garlic's favorite passion is figure skating.", 1.30f, 565.0f, 130.0f, WHITE },
        { "Cabbage-pult","CABBAGEPULT",  "assets/reanim/Cabbagepult.reanim", "anim_idle",      AlmanacGroundType::Roof, 100, "Fast",      "Normal",         "Lobs cabbages at oncoming enemies.",                                                              "Cabbage-pult is cool with lobbing cabbages.", 1.20f, 581.0f, 145.0f, WHITE },
        { "Cornpult",     "CORNPULT",     "assets/reanim/Cornpult.reanim",    "anim_idle",      AlmanacGroundType::Roof, 100, "Fast",      "Normal / Stun",  "Lobs kernels and butter that temporarily immobilizes zombies.",                                   "Cornpult is the oldest of the Pult siblings.", 1.20f, 581.0f, 145.0f, WHITE },
        { "Melonpult",    "MELONPULT",    "assets/reanim/Melonpult.reanim",   "anim_idle",      AlmanacGroundType::Roof, 300, "Fast",      "Heavy",          "Lobs heavy melons that do massive splash damage across multiple lanes.",                          "Melonpult does not mess around when it comes to defending the roof.", 1.20f, 597.0f, 143.0f, WHITE },
        { "Gatling Pea",  "GATLINGPEA",   "assets/reanim/GatlingPea.reanim",  "anim_head_idle", AlmanacGroundType::Day,  250, "Very Slow", "4x Normal",      "Shoots four peas at a rapid firing rate.",                                                        "Gatling Pea's parents were worried when he decided to join the army.", 1.30f, 558.0f, 125.0f, WHITE },
        { "Plantern",     "PLANTERN",     "assets/reanim/Plantern.reanim",    "anim_idle",      AlmanacGroundType::Night, 25,  "Slow",      "None",           "Planterns light up an area, letting you see through fog.",                                        "Plantern defies science. He just does. Other plants eat light and produce oxygen; Plantern eats darkness and produces light.", 1.25f, 565.0f, 130.0f, WHITE },
        { "Twin Sunflower","TWINSUNFLOWER","assets/reanim/TwinSunflower.reanim","anim_idle",   AlmanacGroundType::Day,  125, "Very Slow", "Double",         "Twin Sunflowers produce twice as much sun as a normal sunflower.",                                 "It was a crazed night of forbidden science that brought Twin Sunflower to life. Thunder raged, strange lights flickered, even the very roaring wind seemed to hiss, 'TWIN SUNFLOWER LIVES!'", 1.25f, 565.0f, 125.0f, WHITE }
    };

    // 7 Implemented Zombies
    m_zombies = {
        { "Regular Zombie",        "assets/reanim/Zombie.reanim",               "anim_walk", AlmanacGroundType::Day,   "Low",       "Basic",                    "Regular garden-variety zombie.",                                                                   "This zombie loves brains. Can't get enough of them.", 1.00f, 577.0f, 155.0f, 0.62f, 16.0f, 10.0f },
        { "Flag Zombie",           "assets/reanim/Zombie.reanim",               "anim_walk", AlmanacGroundType::Day,   "Low",       "Basic",                    "Marks the arrival of a huge wave of zombies.",                                                     "Make no mistake, Flag Zombie loves brains just as much as the next guy.", 1.00f, 583.0f, 155.0f, 0.62f, 16.0f, 10.0f },
        { "Conehead Zombie",       "assets/reanim/Zombie.reanim",               "anim_walk", AlmanacGroundType::Day,   "Medium",    "Basic",                    "His traffic cone makes him twice as tough as a regular zombie.",                                   "Conehead Zombie was minding his own business when an emergency traffic cone landed on him.", 1.00f, 577.0f, 166.0f, 0.55f, 17.0f, 14.0f },
        { "Pole Vaulting Zombie",  "assets/reanim/Zombie_polevaulter.reanim",   "anim_run",  AlmanacGroundType::Day,   "Medium",    "Fast",                     "Vaults over the first plant he encounters.",                                                       "Pole Vaulting Zombie committed himself to excellence in pole vaulting.", 0.95f, 576.0f, 155.0f, 0.60f, 18.0f, 10.0f },
        { "Buckethead Zombie",     "assets/reanim/Zombie.reanim",               "anim_walk", AlmanacGroundType::Day,   "High",      "Basic",                    "His bucket head makes him extremely durable and resistant to damage.",                             "Buckethead Zombie always wore a bucket. In a world of zombies, it made him feel unique.", 1.00f, 579.0f, 162.0f, 0.58f, 17.0f, 12.0f },
        { "Football Zombie",       "assets/reanim/Zombie_football.reanim",      "anim_walk", AlmanacGroundType::Day,   "Very High", "Fast",                     "Heavy helmet and rapid sprint make him a devastating threat on the lawn.",                         "Football Zombie gives 110 percent on the field, though he doesn't know what a football is.", 0.90f, 576.0f, 158.0f, 0.52f, 12.0f, 10.0f },
        { "Newspaper Zombie",      "assets/reanim/Zombie_paper.reanim",         "anim_walk", AlmanacGroundType::Night, "Medium",    "Basic (Fast when angry)",  "His newspaper protects him until it's destroyed, sending him into an angry sprint.",               "Newspaper Zombie was this close to finishing his Sudoku puzzle.", 0.95f, 579.0f, 153.0f, 0.58f, 18.0f, 10.0f }
    };

    // Initialize still Reanimations for zombie icon buttons (frozen still image at frame 0)
    std::string flagPath = m_res.GetAssetPath("assets/reanim/Zombie_flagpole.reanim");
    if (FileExists(flagPath.c_str())) {
        m_iconFlagAnim.SetResources(m_res.LoadReanim(flagPath), m_res);
        m_iconFlagAnim.SetPaused(true);
    }

    m_zombieIconAnims.clear();
    for (size_t i = 0; i < m_zombies.size(); ++i) {
        const auto& z = m_zombies[i];
        std::string p = m_res.GetAssetPath(z.reanimPath);
        if (FileExists(p.c_str())) {
            Reanimation anim;
            ReanimDefinition def = m_res.LoadReanim(p);
            anim.SetResources(def, m_res);
            anim.SetBaseAnimation(z.defaultAnim);
            anim.SetAnimation(z.defaultAnim);
            anim.SetPaused(true);

            // Configure armored and accessory tracks for still icon
            if (z.name == "Regular Zombie") {
                anim.SetTrackVisible("anim_bucket", false);
                anim.SetTrackVisible("anim_cone", false);
                anim.SetTrackVisible("anim_screendoor", false);
                anim.SetTrackVisible("Zombie_duckytube", false);
                anim.SetTrackVisible("Zombie_mustache", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_flaghand", false);
                anim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (z.name == "Flag Zombie") {
                anim.SetTrackVisible("anim_bucket", false);
                anim.SetTrackVisible("anim_cone", false);
                anim.SetTrackVisible("anim_screendoor", false);
                anim.SetTrackVisible("Zombie_duckytube", false);
                anim.SetTrackVisible("Zombie_mustache", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor", true);
                anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                anim.SetTrackVisible("anim_innerarm1", false);
                anim.SetTrackVisible("anim_innerarm2", false);
                anim.SetTrackVisible("anim_innerarm3", false);
                anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_flaghand", true);
                anim.SetTrackVisible("Zombie_outerarm_upper", true);
                anim.SetTrackVisible("Zombie_outerarm_lower", true);
                anim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (z.name == "Conehead Zombie") {
                anim.SetTrackVisible("anim_bucket", false);
                anim.SetTrackVisible("anim_cone", true);
                anim.SetTrackVisible("anim_screendoor", false);
                anim.SetTrackVisible("Zombie_duckytube", false);
                anim.SetTrackVisible("Zombie_mustache", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_flaghand", false);
                anim.SetTrackVisible("Zombie_outerarm_hand", true);
            } else if (z.name == "Buckethead Zombie") {
                anim.SetTrackVisible("anim_bucket", true);
                anim.SetTrackVisible("anim_cone", false);
                anim.SetTrackVisible("anim_screendoor", false);
                anim.SetTrackVisible("Zombie_duckytube", false);
                anim.SetTrackVisible("Zombie_mustache", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                anim.SetTrackVisible("Zombie_flaghand", false);
                anim.SetTrackVisible("Zombie_outerarm_hand", true);
            }

            m_zombieIconAnims.push_back(anim);
        } else {
            m_zombieIconAnims.push_back(Reanimation());
        }
    }
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
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor", true);
                m_previewAnim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
                m_previewAnim.SetTrackVisible("anim_innerarm1", false);
                m_previewAnim.SetTrackVisible("anim_innerarm2", false);
                m_previewAnim.SetTrackVisible("anim_innerarm3", false);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_screendoor", false);
                m_previewAnim.SetTrackVisible("Zombie_flaghand", true);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_upper", true);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_lower", true);
                m_previewAnim.SetTrackVisible("Zombie_outerarm_hand", true);

                std::string flagPath = m_res.GetAssetPath("assets/reanim/Zombie_flagpole.reanim");
                if (FileExists(flagPath.c_str())) {
                    m_previewFlagAnim = Reanimation();
                    m_previewFlagAnim.SetResources(m_res.LoadReanim(flagPath), m_res);
                }
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
        Rectangle indexBtnRect = { 180.0f, 569.0f, 120.0f, 26.0f };
        if (isButtonHovered(mousePos, indexBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        Rectangle switchBtnRect = { 350.0f, 569.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, switchBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Zombies;
            loadSelectedZombiePreview();
            return;
        }

        Rectangle closeBtnRect = { 660.0f, 569.0f, 89.0f, 26.0f };
        if (isButtonHovered(mousePos, closeBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxClick);
            showAlmanac = false;
            return;
        }
    } else if (m_currentPage == AlmanacPage::Zombies) {
        m_previewAnim.Update(dt);
        if (m_selectedZombie >= 0 && m_selectedZombie < (int)m_zombies.size() && m_zombies[m_selectedZombie].name == "Flag Zombie") {
            m_previewFlagAnim.Update(dt);
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        // Check zombie grid clicks (5 cols x 6 rows)
        float startX = 35.0f;
        float startY = 125.0f;
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
        Rectangle indexBtnRect = { 180.0f, 569.0f, 120.0f, 26.0f };
        if (isButtonHovered(mousePos, indexBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Index;
            return;
        }

        Rectangle switchBtnRect = { 350.0f, 569.0f, 164.0f, 26.0f };
        if (isButtonHovered(mousePos, switchBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(sfxPaper);
            m_currentPage = AlmanacPage::Plants;
            loadSelectedPlantPreview();
            return;
        }

        Rectangle closeBtnRect = { 660.0f, 569.0f, 89.0f, 26.0f };
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

        // Animated Mini Models centered in their respective rectangular stone windows
        m_indexPlantAnim.Draw(158.0f, 230.0f, 1.40f);
        m_indexZombieAnim.Draw(542.0f, 218.0f, 1.10f);

        // Title
        m_titleFont.DrawTextCentered("Suburban Almanac - Index", { 0.0f, 22.0f, 800.0f, 50.0f }, 1.1f, Color{ 240, 200, 80, 255 });

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
        m_titleFont.DrawTextCentered("Suburban Almanac - Plants", { 0.0f, 16.0f, 800.0f, 42.0f }, 0.95f, Color{ 230, 200, 80, 255 });

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
            Texture2D groundTex = m_groundDay;
            if (plant.ground == AlmanacGroundType::Night) {
                groundTex = m_groundNight;
            } else if (plant.ground == AlmanacGroundType::Roof) {
                groundTex = m_groundRoof;
            } else if (plant.ground == AlmanacGroundType::Pool) {
                groundTex = m_groundPool;
            }
            if (groundTex.id != 0) {
                DrawTexturePro(
                    groundTex,
                    { 0.0f, 0.0f, (float)groundTex.width, (float)groundTex.height },
                    { 516.0f, 94.0f, 200.0f, 160.0f },
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }

            // 2. Live Animated Plant Preview centered in the upper slot
            m_previewAnim.Draw(plant.previewOffsetX, plant.previewOffsetY, plant.animScale, plant.tint);

            // 3. Parchment Card Frame overlay
            if (m_plantCard.id != 0) {
                DrawTexture(m_plantCard, 458, 85, WHITE);
            }

            // 4. Plant Name (Yellow)
            m_headerFont.DrawTextCentered(plant.name.c_str(), { 466.0f, 264.0f, 305.0f, 28.0f }, 0.95f, Color{ 255, 220, 50, 255 });

            // Helper lambda for two-tone stat lines (Brown label + Red value)
            auto drawStatLine = [&](const std::string& label, const std::string& val, float x, float y, float scale) {
                std::string fullLabel = label + ": ";
                m_bodyFont.DrawText(fullLabel.c_str(), x, y, scale, Color{ 40, 30, 20, 255 });
                int labelW = m_bodyFont.MeasureText(fullLabel.c_str(), scale);
                m_bodyFont.DrawText(val.c_str(), x + (float)labelW, y, scale, Color{ 220, 30, 30, 255 });
            };

            // 5. Stats
            drawStatLine("Cost", std::to_string(plant.sunCost), 492.0f, 318.0f, 0.75f);
            drawStatLine("Recharge", plant.recharge, 492.0f, 338.0f, 0.75f);
            drawStatLine("Damage", plant.damage, 492.0f, 358.0f, 0.75f);

            // 6. Description & Flavor Story (Multiline Wrapped)
            drawWrappedText(m_bodyFont, plant.description, 492.0f, 385.0f, 240.0f, 16.0f, 0.72f, Color{ 40, 30, 20, 255 });
            drawWrappedText(m_bodyFont, plant.flavor,      492.0f, 450.0f, 240.0f, 15.0f, 0.65f, Color{ 110, 85, 60, 255 });
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 569.0f, 120.0f, 26.0f };
        bool indexHover = isButtonHovered(mousePos, indexBtnRect);
        drawButton(indexBtnRect, "INDEX", m_indexBtn, m_indexBtnHl, indexHover);

        Rectangle switchBtnRect = { 350.0f, 569.0f, 164.0f, 26.0f };
        bool switchHover = isButtonHovered(mousePos, switchBtnRect);
        drawButton(switchBtnRect, "VIEW ZOMBIES", m_indexBtn, m_indexBtnHl, switchHover);

        Rectangle closeBtnRect = { 660.0f, 569.0f, 89.0f, 26.0f };
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
        m_headerFont.DrawTextCentered("Suburban Almanac - Zombies", { 0.0f, 25.0f, 800.0f, 40.0f }, 0.95f, Color{ 230, 200, 80, 255 });

        // Left Grid of Zombie Windows (5 cols x 6 rows = 30 slots)
        float startX = 35.0f;
        float startY = 125.0f;
        float slotW  = 64.0f;
        float slotH  = 64.0f;
        int cols     = 5;
        int totalSlots = 30;

        for (int i = 0; i < totalSlots; ++i) {
            int row = i / cols;
            int col = i % cols;
            Rectangle slotRect = { startX + col * 68.0f, startY + row * 68.0f, slotW, slotH };

            if (i < (int)m_zombies.size()) {
                // 1. Draw base window plate (Almanac_ZombieWindow.png)
                if (m_zombieWindow.id != 0) {
                    DrawTexturePro(
                        m_zombieWindow,
                        { 0.0f, 0.0f, (float)m_zombieWindow.width, (float)m_zombieWindow.height },
                        slotRect,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                }

                // 2. Scissor clip to inner area of window so zombie stays completely inside the frame without protruding
                float padX = 6.0f;
                float padY = 6.0f;
                Rectangle innerClip = {
                    slotRect.x + padX,
                    slotRect.y + padY,
                    slotRect.width - padX * 2.0f,
                    slotRect.height - padY * 2.0f
                };

                BeginScissorMode((int)innerClip.x, (int)innerClip.y, (int)innerClip.width, (int)innerClip.height);

                if (i < (int)m_zombieIconAnims.size()) {
                    const auto& zData = m_zombies[i];
                    m_zombieIconAnims[i].Draw(
                        slotRect.x + zData.iconOffsetX,
                        slotRect.y + zData.iconOffsetY,
                        zData.iconScale
                    );

                    if (zData.name == "Flag Zombie") {
                        float handX = -9.2f, handY = 50.1f, handRot = 0.0f;
                        if (m_zombieIconAnims[i].GetTrackTransform("Zombie_flaghand", handX, handY, handRot)) {
                            float dx = handX - (-9.2f);
                            float dy = handY - 50.1f;
                            m_iconFlagAnim.Draw(
                                slotRect.x + zData.iconOffsetX + dx * zData.iconScale,
                                slotRect.y + zData.iconOffsetY + dy * zData.iconScale,
                                zData.iconScale
                            );
                        } else {
                            m_iconFlagAnim.Draw(
                                slotRect.x + zData.iconOffsetX,
                                slotRect.y + zData.iconOffsetY,
                                zData.iconScale
                            );
                        }
                    }
                }
                EndScissorMode();

                // 3. Overlay metal frame border (Almanac_ZombieWindow2.png)
                if (m_zombieWindow2.id != 0) {
                    DrawTexturePro(
                        m_zombieWindow2,
                        { 0.0f, 0.0f, (float)m_zombieWindow2.width, (float)m_zombieWindow2.height },
                        slotRect,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                }

                // 4. Selection / Hover highlight
                if (i == m_selectedZombie) {
                    DrawRectangleLinesEx(slotRect, 3.0f, GOLD);
                } else if (CheckCollisionPointRec(mousePos, slotRect)) {
                    DrawRectangleLinesEx(slotRect, 2.0f, WHITE);
                }
            } else {
                // Empty blank slot (render Almanac_ZombieBlank.png only)
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

        // Right Display Card (Ground Tile -> Animated Zombie -> Parchment Frame Overlay -> Text)
        if (m_selectedZombie >= 0 && m_selectedZombie < (int)m_zombies.size()) {
            const auto& zombie = m_zombies[m_selectedZombie];

            // 1. Ground Tile behind zombie
            Texture2D groundTex = m_groundDay;
            if (zombie.ground == AlmanacGroundType::Night) {
                groundTex = m_groundNight;
            } else if (zombie.ground == AlmanacGroundType::Roof) {
                groundTex = m_groundRoof;
            } else if (zombie.ground == AlmanacGroundType::Pool) {
                groundTex = m_groundPool;
            }
            if (groundTex.id != 0) {
                DrawTexturePro(
                    groundTex,
                    { 0.0f, 0.0f, (float)groundTex.width, (float)groundTex.height },
                    { 518.0f, 126.0f, 204.0f, 184.0f },
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }

            // 2. Live Animated Preview centered in the upper slot
            m_previewAnim.Draw(zombie.previewOffsetX, zombie.previewOffsetY, zombie.animScale);
            if (zombie.name == "Flag Zombie") {
                float handX = -9.2f, handY = 50.1f, handRot = 0.0f;
                if (m_previewAnim.GetTrackTransform("Zombie_flaghand", handX, handY, handRot)) {
                    float dx = handX - (-9.2f);
                    float dy = handY - 50.1f;
                    m_previewFlagAnim.Draw(
                        zombie.previewOffsetX + dx * zombie.animScale,
                        zombie.previewOffsetY + dy * zombie.animScale,
                        zombie.animScale
                    );
                } else {
                    m_previewFlagAnim.Draw(zombie.previewOffsetX, zombie.previewOffsetY, zombie.animScale);
                }
            }

            // 3. Parchment Card Frame overlay
            if (m_zombieCard.id != 0) {
                DrawTexture(m_zombieCard, 458, 78, WHITE);
            }

            // 3. Zombie Name (Centered in the stone space between the shiny slanted border of the upper window and lavender parchment)
            m_headerFont.DrawTextCentered(zombie.name.c_str(), { 466.0f, 343.0f, 305.0f, 24.0f }, 0.95f, Color{ 255, 220, 50, 255 });

            // Helper lambda for two-tone stat lines (Brown label + Red value)
            auto drawStatLine = [&](const std::string& label, const std::string& val, float x, float y, float scale) {
                std::string fullLabel = label + ": ";
                m_bodyFont.DrawText(fullLabel.c_str(), x, y, scale, Color{ 40, 30, 20, 255 });
                int labelW = m_bodyFont.MeasureText(fullLabel.c_str(), scale);
                m_bodyFont.DrawText(val.c_str(), x + (float)labelW, y, scale, Color{ 220, 30, 30, 255 });
            };

            // 4. Stats (Inside lavender parchment with large padding)
            drawStatLine("Toughness", zombie.toughness, 494.0f, 392.0f, 0.72f);
            drawStatLine("Speed", zombie.speed, 494.0f, 410.0f, 0.72f);

            // 5. Description & Flavor Story (Multiline Wrapped)
            drawWrappedText(m_bodyFont, zombie.description, 494.0f, 435.0f, 240.0f, 15.0f, 0.70f, Color{ 40, 30, 20, 255 });
            drawWrappedText(m_bodyFont, zombie.flavor,      494.0f, 485.0f, 240.0f, 14.0f, 0.65f, Color{ 110, 85, 60, 255 });
        }

        // Bottom Navigation Buttons
        Rectangle indexBtnRect = { 180.0f, 569.0f, 120.0f, 26.0f };
        bool indexHover = isButtonHovered(mousePos, indexBtnRect);
        drawButton(indexBtnRect, "INDEX", m_indexBtn, m_indexBtnHl, indexHover);

        Rectangle switchBtnRect = { 350.0f, 569.0f, 164.0f, 26.0f };
        bool switchHover = isButtonHovered(mousePos, switchBtnRect);
        drawButton(switchBtnRect, "VIEW PLANTS", m_indexBtn, m_indexBtnHl, switchHover);

        Rectangle closeBtnRect = { 660.0f, 569.0f, 89.0f, 26.0f };
        bool closeHover = isButtonHovered(mousePos, closeBtnRect);
        drawButton(closeBtnRect, "CLOSE", m_closeBtn, m_closeBtnHl, closeHover);
    }
}
