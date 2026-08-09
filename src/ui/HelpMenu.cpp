#include "HelpMenu.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <iostream>

HelpMenu::HelpMenu(Resources& res)
    : m_res(res) {
    m_zombieNote          = res.GetTexture("ZOMBIENOTE");
    m_zombieNoteHelpBlack = res.GetTexture("ZOMBIENOTEHELPBLACK");
}

void HelpMenu::update(float dt, bool& showHelp) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::string sfxPath = m_res.GetAssetPath("assets/sounds/gravebutton.ogg");
        AudioManager::GetInstance().PlaySoundEffect(sfxPath);
        showHelp = false;
    }
}

void HelpMenu::draw() {
    // 1. Draw dark background overlay dimming the main menu
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.6f));

    Texture2D noteTex = (m_zombieNote.id != 0) ? m_zombieNote : m_res.GetTexture("ZOMBIENOTE");
    Texture2D textTex = (m_zombieNoteHelpBlack.id != 0) ? m_zombieNoteHelpBlack : m_res.GetTexture("ZOMBIENOTEHELPBLACK");

    // 2. Draw centered Zombie Note paper texture (654x427)
    if (noteTex.id != 0) {
        float noteW = (float)noteTex.width;
        float noteH = (float)noteTex.height;
        float noteX = (800.0f - noteW) / 2.0f;
        float noteY = (600.0f - noteH) / 2.0f;
        DrawTexture(noteTex, (int)noteX, (int)noteY, WHITE);
    }

    // 3. Draw black text overlay (529x323) centered over the paper note
    if (textTex.id != 0) {
        float textW = (float)textTex.width;
        float textH = (float)textTex.height;
        float textX = (800.0f - textW) / 2.0f;
        float textY = (600.0f - textH) / 2.0f;
        DrawTexture(textTex, (int)textX, (int)textY, WHITE);
    }
}
