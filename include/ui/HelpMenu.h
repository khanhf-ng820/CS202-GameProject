#pragma once
#include "raylib.h"
#include "resources.h"

class HelpMenu {
public:
    HelpMenu(Resources& res);
    ~HelpMenu() = default;

    void update(float dt, bool& showHelp);
    void draw();

private:
    Resources& m_res;
    Texture2D m_zombieNote;
    Texture2D m_zombieNoteHelpBlack;
};
