#pragma once
#include "raylib.h"
#include "resources.h"
#include "BitmapFont.h"
#include "ProfileManager.h"
#include <string>
#include <vector>

class UserDialog {
public:
    UserDialog(Resources& res);
    ~UserDialog() = default;

    void Open();
    void Close();
    bool IsOpen() const { return m_isOpen; }

    void Update(float dt);
    void Draw();

private:
    void refreshUserList();

    Resources& m_res;
    bool m_isOpen = false;
    bool m_isCreatingUser = false;

    BitmapFont m_fontTitle;     // HouseofTerror28
    BitmapFont m_fontButton;    // DwarvenTodcraft24
    BitmapFont m_fontSmall;     // BrianneTod16

    std::vector<std::string> m_userNames;
    int m_selectedIndex = 0;
    int m_scrollOffset = 0;
    bool m_isDraggingScrollbar = false;
    float m_dragStartY = 0.0f;
    int m_dragStartOffset = 0;

    // Create user input state
    char m_inputBuffer[32] = { 0 };
    int m_inputLetterCount = 0;
    float m_cursorBlinkTimer = 0.0f;
    std::string m_errorMessage;
};
