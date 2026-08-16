#include "UserDialog.h"
#include "UIHelpers.h"
#include "AudioManager.h"
#include <algorithm>
#include <sstream>

static std::string FormatMoney(int amount) {
    std::string s = std::to_string(amount);
    int n = (int)s.length() - 3;
    while (n > 0) {
        s.insert(n, ",");
        n -= 3;
    }
    return "$" + s;
}

UserDialog::UserDialog(Resources& res)
    : m_res(res), m_isOpen(false), m_isCreatingUser(false) {
    
    // Load bitmap fonts
    std::string hotPng = res.GetAssetPath("assets/data/HouseofTerror28.png");
    std::string hotTxt = res.GetAssetPath("assets/data/HouseofTerror28.txt");
    m_fontTitle.Load(hotPng, hotTxt);

    std::string todcraftPng = res.GetAssetPath("assets/data/DwarvenTodcraft24.png");
    std::string todcraftTxt = res.GetAssetPath("assets/data/DwarvenTodcraft24.txt");
    m_fontButton.Load(todcraftPng, todcraftTxt);

    std::string briannePng = res.GetAssetPath("assets/data/_BrianneTod16.png");
    std::string brianneTxt = res.GetAssetPath("assets/data/BrianneTod16.txt");
    m_fontSmall.Load(briannePng, brianneTxt);
}

void UserDialog::refreshUserList() {
    m_userNames = ProfileManager::GetInstance().GetAllUserNames();
    std::string active = ProfileManager::GetInstance().GetActiveUserName();
    
    m_selectedIndex = 0;
    for (size_t i = 0; i < m_userNames.size(); ++i) {
        if (m_userNames[i] == active) {
            m_selectedIndex = (int)i;
            break;
        }
    }
}

void UserDialog::Open() {
    m_isOpen = true;
    m_isCreatingUser = false;
    m_errorMessage.clear();
    m_inputBuffer[0] = '\0';
    m_inputLetterCount = 0;
    m_cursorBlinkTimer = 0.0f;
    refreshUserList();
    AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/paper.ogg"));
}

void UserDialog::Close() {
    m_isOpen = false;
    m_isCreatingUser = false;
    m_errorMessage.clear();
}

void UserDialog::Update(float dt) {
    if (!m_isOpen) return;

    Vector2 mousePos = GetVirtualMousePosition();
    m_cursorBlinkTimer += dt;

    if (m_isCreatingUser) {
        // Handle character typing
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (m_inputLetterCount < 14)) {
                m_inputBuffer[m_inputLetterCount] = (char)key;
                m_inputBuffer[m_inputLetterCount + 1] = '\0';
                m_inputLetterCount++;
                m_errorMessage.clear();
            }
            key = GetCharPressed();
        }

        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (m_inputLetterCount > 0) {
                m_inputLetterCount--;
                m_inputBuffer[m_inputLetterCount] = '\0';
                m_errorMessage.clear();
            }
        }

        // Handle enter to submit
        if (IsKeyPressed(KEY_ENTER)) {
            std::string newName(m_inputBuffer);
            std::string err;
            if (ProfileManager::GetInstance().CreateUser(newName, err)) {
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
                m_isCreatingUser = false;
                m_errorMessage.clear();
                m_inputBuffer[0] = '\0';
                m_inputLetterCount = 0;
                refreshUserList();
                int maxOffset = std::max(0, (int)m_userNames.size() - 4);
                if (m_selectedIndex >= 4) {
                    m_scrollOffset = std::min(maxOffset, m_selectedIndex);
                }
                return;
            } else {
                m_errorMessage = err;
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/buzzer.ogg"));
            }
        }

        // Handle escape to cancel
        if (IsKeyPressed(KEY_ESCAPE)) {
            m_isCreatingUser = false;
            m_errorMessage.clear();
            return;
        }

        // Button: [ CREATE ]
        Rectangle createBtnRect = { 230.0f, 360.0f, 155.0f, 44.0f };
        if (CheckCollisionPointRec(mousePos, createBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::string newName(m_inputBuffer);
            std::string err;
            if (ProfileManager::GetInstance().CreateUser(newName, err)) {
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
                m_isCreatingUser = false;
                m_errorMessage.clear();
                m_inputBuffer[0] = '\0';
                m_inputLetterCount = 0;
                refreshUserList();
                int maxOffset = std::max(0, (int)m_userNames.size() - 4);
                if (m_selectedIndex >= 4) {
                    m_scrollOffset = std::min(maxOffset, m_selectedIndex);
                }
                return;
            } else {
                m_errorMessage = err;
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/buzzer.ogg"));
            }
        }

        // Button: [ CANCEL ]
        Rectangle cancelBtnRect = { 415.0f, 360.0f, 155.0f, 44.0f };
        if (CheckCollisionPointRec(mousePos, cancelBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            m_isCreatingUser = false;
            m_errorMessage.clear();
        }

    } else {
        // User selection mode
        int maxOffset = std::max(0, (int)m_userNames.size() - 4);

        // 1. Mouse wheel scrolling
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            m_scrollOffset -= (int)wheel;
            m_scrollOffset = std::clamp(m_scrollOffset, 0, maxOffset);
        }

        // 2. Scrollbar Up/Down Buttons & Dragging
        bool hasScrollbar = (m_userNames.size() > 4);
        if (hasScrollbar) {
            Rectangle upBtnRect   = { 595.0f, 155.0f, 20.0f, 20.0f };
            Rectangle downBtnRect = { 595.0f, 375.0f, 20.0f, 20.0f };
            Rectangle trackRect   = { 595.0f, 177.0f, 20.0f, 196.0f };

            float thumbH = std::max(28.0f, 196.0f * (4.0f / (float)m_userNames.size()));
            float thumbY = 177.0f + ((float)m_scrollOffset / (float)maxOffset) * (196.0f - thumbH);
            Rectangle thumbRect = { 595.0f, thumbY, 20.0f, thumbH };

            if (CheckCollisionPointRec(mousePos, upBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_scrollOffset = std::max(0, m_scrollOffset - 1);
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/tap.ogg"));
            } else if (CheckCollisionPointRec(mousePos, downBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_scrollOffset = std::min(maxOffset, m_scrollOffset + 1);
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/tap.ogg"));
            }

            if (CheckCollisionPointRec(mousePos, thumbRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_isDraggingScrollbar = true;
                m_dragStartY = mousePos.y;
                m_dragStartOffset = m_scrollOffset;
            }

            if (m_isDraggingScrollbar) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    float deltaY = mousePos.y - m_dragStartY;
                    float availableTravel = 196.0f - thumbH;
                    if (availableTravel > 0.0f) {
                        float offsetChange = (deltaY / availableTravel) * (float)maxOffset;
                        m_scrollOffset = std::clamp(m_dragStartOffset + (int)offsetChange, 0, maxOffset);
                    }
                } else {
                    m_isDraggingScrollbar = false;
                }
            } else if (CheckCollisionPointRec(mousePos, trackRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Click on track above or below thumb
                if (mousePos.y < thumbY) {
                    m_scrollOffset = std::max(0, m_scrollOffset - 2);
                } else if (mousePos.y > thumbY + thumbH) {
                    m_scrollOffset = std::min(maxOffset, m_scrollOffset + 2);
                }
            }
        } else {
            m_scrollOffset = 0;
            m_isDraggingScrollbar = false;
        }

        // 3. Check clicking visible items in list
        float slotW = hasScrollbar ? 395.0f : 430.0f;
        for (int k = 0; k < 4; ++k) {
            int actualIdx = m_scrollOffset + k;
            if (actualIdx >= (int)m_userNames.size()) break;

            Rectangle slotRect = { 185.0f, 155.0f + (float)k * 56.0f, slotW, 50.0f };
            if (CheckCollisionPointRec(mousePos, slotRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_selectedIndex = actualIdx;
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/tap.ogg"));
            }
        }

        // 4. Action Buttons
        // [ OK / SELECT ]
        Rectangle okBtnRect = { 185.0f, 415.0f, 95.0f, 42.0f };
        if (CheckCollisionPointRec(mousePos, okBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_userNames.size()) {
                ProfileManager::GetInstance().SetActiveUser(m_userNames[m_selectedIndex]);
                AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
                Close();
                return;
            }
        }

        // [ NEW USER ]
        Rectangle newBtnRect = { 290.0f, 415.0f, 125.0f, 42.0f };
        if (CheckCollisionPointRec(mousePos, newBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            m_isCreatingUser = true;
            m_inputBuffer[0] = '\0';
            m_inputLetterCount = 0;
            m_errorMessage.clear();
        }

        // [ DELETE ]
        Rectangle delBtnRect = { 425.0f, 415.0f, 95.0f, 42.0f };
        bool canDelete = (m_selectedIndex >= 0 && m_selectedIndex < (int)m_userNames.size() && m_userNames[m_selectedIndex] != "admin");
        if (canDelete && CheckCollisionPointRec(mousePos, delBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            ProfileManager::GetInstance().DeleteUser(m_userNames[m_selectedIndex]);
            refreshUserList();
        }

        // [ CANCEL ]
        Rectangle cancelBtnRect = { 530.0f, 415.0f, 85.0f, 42.0f };
        if (CheckCollisionPointRec(mousePos, cancelBtnRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            AudioManager::GetInstance().PlaySoundEffect(m_res.GetAssetPath("assets/sounds/gravebutton.ogg"));
            Close();
        }
    }
}

void UserDialog::Draw() {
    if (!m_isOpen) return;

    Vector2 mousePos = GetVirtualMousePosition();

    // 1. Semi-transparent backdrop overlay
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.7f));

    // 2. Main Dialog Frame
    Rectangle dialogRect = { 160.0f, 85.0f, 480.0f, 400.0f };
    DrawRectangleRec(dialogRect, Color{ 35, 25, 20, 250 });
    DrawRectangleLinesEx(dialogRect, 3.5f, GOLD);
    DrawRectangleLinesEx({ dialogRect.x + 4, dialogRect.y + 4, dialogRect.width - 8, dialogRect.height - 8 }, 1.5f, Color{ 160, 120, 60, 255 });

    // Dialog Header
    Rectangle headerRect = { dialogRect.x, dialogRect.y + 18.0f, dialogRect.width, 35.0f };
    std::string titleStr = m_isCreatingUser ? "NEW USER PROFILE" : "WHO ARE YOU?";
    m_fontTitle.DrawTextCentered(titleStr.c_str(), { headerRect.x + 2, headerRect.y + 2, headerRect.width, headerRect.height }, 0.65f, BLACK);
    m_fontTitle.DrawTextCentered(titleStr.c_str(), headerRect, 0.65f, YELLOW);

    if (m_isCreatingUser) {
        // --- Create New User UI ---
        float promptY = dialogRect.y + 75.0f;
        m_fontSmall.DrawTextCentered("Enter your player name below:", { dialogRect.x, promptY, dialogRect.width, 25.0f }, 1.05f, WHITE);

        // Input Box
        Rectangle inputRect = { 200.0f, dialogRect.y + 130.0f, 400.0f, 50.0f };
        DrawRectangleRec(inputRect, ColorAlpha(BLACK, 0.85f));
        DrawRectangleLinesEx(inputRect, 2.5f, GOLD);

        // Display typed text + blinking cursor
        std::string displayText = m_inputBuffer;
        if (((int)(m_cursorBlinkTimer * 2.0f) % 2) == 0) {
            displayText += "_";
        }
        m_fontButton.DrawText(displayText.c_str(), inputRect.x + 15.0f, inputRect.y + 12.0f, 0.8f, WHITE);

        // Character limit helper
        std::string countStr = std::to_string(m_inputLetterCount) + " / 14";
        m_fontSmall.DrawText(countStr.c_str(), inputRect.x + inputRect.width - 60.0f, inputRect.y + inputRect.height + 6.0f, 0.85f, GRAY);

        // Error message banner
        if (!m_errorMessage.empty()) {
            Rectangle errRect = { dialogRect.x, dialogRect.y + 215.0f, dialogRect.width, 30.0f };
            m_fontSmall.DrawTextCentered(m_errorMessage.c_str(), errRect, 1.05f, RED);
        }

        // Action Buttons: [ CREATE ] & [ CANCEL ]
        Rectangle createBtnRect = { 230.0f, 360.0f, 155.0f, 44.0f };
        bool createHovered = CheckCollisionPointRec(mousePos, createBtnRect);
        DrawRectangleRec(createBtnRect, createHovered ? Color{ 60, 160, 60, 255 } : Color{ 40, 110, 40, 255 });
        DrawRectangleLinesEx(createBtnRect, 2.0f, createHovered ? WHITE : GOLD);
        m_fontButton.DrawTextCentered("CREATE", createBtnRect, 0.75f, WHITE);

        Rectangle cancelBtnRect = { 415.0f, 360.0f, 155.0f, 44.0f };
        bool cancelHovered = CheckCollisionPointRec(mousePos, cancelBtnRect);
        DrawRectangleRec(cancelBtnRect, cancelHovered ? Color{ 160, 60, 60, 255 } : Color{ 110, 40, 40, 255 });
        DrawRectangleLinesEx(cancelBtnRect, 2.0f, cancelHovered ? WHITE : GOLD);
        m_fontButton.DrawTextCentered("CANCEL", cancelBtnRect, 0.75f, WHITE);

    } else {
        // --- Select Existing User UI ---
        std::string activeName = ProfileManager::GetInstance().GetActiveUserName();
        bool hasScrollbar = (m_userNames.size() > 4);
        float slotW = hasScrollbar ? 395.0f : 430.0f;

        // Render each visible user slot (4 visible slots)
        for (int k = 0; k < 4; ++k) {
            int actualIdx = m_scrollOffset + k;
            if (actualIdx >= (int)m_userNames.size()) break;

            Rectangle slotRect = { 185.0f, 155.0f + (float)k * 56.0f, slotW, 50.0f };
            bool isHovered = CheckCollisionPointRec(mousePos, slotRect);
            bool isSelected = (actualIdx == m_selectedIndex);
            bool isActive = (m_userNames[actualIdx] == activeName);

            Color bgCol = ColorAlpha(BLACK, 0.55f);
            Color borderCol = Color{ 120, 90, 50, 255 };
            Color textCol = WHITE;

            if (isSelected) {
                bgCol = ColorAlpha(Color{ 80, 70, 25, 255 }, 0.9f);
                borderCol = GOLD;
                textCol = YELLOW;
            } else if (isHovered) {
                bgCol = ColorAlpha(Color{ 50, 45, 30, 255 }, 0.8f);
                borderCol = Color{ 180, 150, 80, 255 };
            }

            DrawRectangleRec(slotRect, bgCol);
            DrawRectangleLinesEx(slotRect, isSelected ? 2.5f : 1.5f, borderCol);

            // User Name
            std::string nameLabel = m_userNames[actualIdx];
            if (isActive) {
                nameLabel += " [ACTIVE]";
            }
            m_fontButton.DrawText(nameLabel.c_str(), slotRect.x + 15.0f, slotRect.y + 13.0f, 0.75f, textCol);

            // User quick stats (coins & plants) on right side, shifted left so it's clearly visible
            UserProfile prof;
            if (ProfileManager::GetInstance().GetProfile(m_userNames[actualIdx], prof)) {
                std::string statStr = FormatMoney(prof.coins) + " | " + std::to_string(prof.unlockedPlants.size()) + " Plants";
                Color statCol = isSelected ? Color{ 255, 230, 130, 255 } : (m_userNames[actualIdx] == "admin" ? GOLD : Color{ 190, 220, 190, 255 });
                m_fontSmall.DrawText(statStr.c_str(), slotRect.x + slotW - 190.0f, slotRect.y + 17.0f, 0.85f, statCol);
            }
        }

        // Render Scrollbar if more than 4 users
        if (hasScrollbar) {
            int maxOffset = (int)m_userNames.size() - 4;
            Rectangle upBtnRect   = { 595.0f, 155.0f, 20.0f, 20.0f };
            Rectangle downBtnRect = { 595.0f, 375.0f, 20.0f, 20.0f };
            Rectangle trackRect   = { 595.0f, 177.0f, 20.0f, 196.0f };

            // Draw track
            DrawRectangleRec(trackRect, ColorAlpha(BLACK, 0.6f));
            DrawRectangleLinesEx(trackRect, 1.0f, Color{ 100, 80, 40, 255 });

            // Draw Up/Down buttons
            bool upHovered = CheckCollisionPointRec(mousePos, upBtnRect);
            DrawRectangleRec(upBtnRect, upHovered ? Color{ 100, 85, 50, 255 } : Color{ 60, 50, 30, 255 });
            DrawRectangleLinesEx(upBtnRect, 1.5f, upHovered ? WHITE : GOLD);
            m_fontSmall.DrawTextCentered("^", upBtnRect, 1.0f, WHITE);

            bool downHovered = CheckCollisionPointRec(mousePos, downBtnRect);
            DrawRectangleRec(downBtnRect, downHovered ? Color{ 100, 85, 50, 255 } : Color{ 60, 50, 30, 255 });
            DrawRectangleLinesEx(downBtnRect, 1.5f, downHovered ? WHITE : GOLD);
            m_fontSmall.DrawTextCentered("v", downBtnRect, 0.8f, WHITE);

            // Draw Thumb
            float thumbH = std::max(28.0f, 196.0f * (4.0f / (float)m_userNames.size()));
            float thumbY = 177.0f + ((float)m_scrollOffset / (float)maxOffset) * (196.0f - thumbH);
            Rectangle thumbRect = { 595.0f, thumbY, 20.0f, thumbH };
            bool thumbHovered = CheckCollisionPointRec(mousePos, thumbRect) || m_isDraggingScrollbar;

            DrawRectangleRec(thumbRect, thumbHovered ? Color{ 180, 140, 60, 255 } : Color{ 130, 100, 45, 255 });
            DrawRectangleLinesEx(thumbRect, 1.5f, thumbHovered ? WHITE : GOLD);
        }

        // Bottom Action Buttons
        // 1. [ OK / SELECT ]
        Rectangle okBtnRect = { 185.0f, 420.0f, 95.0f, 42.0f };
        bool okHovered = CheckCollisionPointRec(mousePos, okBtnRect);
        DrawRectangleRec(okBtnRect, okHovered ? Color{ 50, 160, 50, 255 } : Color{ 35, 110, 35, 255 });
        DrawRectangleLinesEx(okBtnRect, 2.0f, okHovered ? WHITE : GOLD);
        m_fontButton.DrawTextCentered("OK", okBtnRect, 0.75f, WHITE);

        // 2. [ NEW USER ]
        Rectangle newBtnRect = { 290.0f, 420.0f, 125.0f, 42.0f };
        bool newHovered = CheckCollisionPointRec(mousePos, newBtnRect);
        DrawRectangleRec(newBtnRect, newHovered ? Color{ 60, 120, 180, 255 } : Color{ 35, 80, 130, 255 });
        DrawRectangleLinesEx(newBtnRect, 2.0f, newHovered ? WHITE : GOLD);
        m_fontButton.DrawTextCentered("NEW USER", newBtnRect, 0.65f, WHITE);

        // 3. [ DELETE ]
        Rectangle delBtnRect = { 425.0f, 420.0f, 95.0f, 42.0f };
        bool canDelete = (m_selectedIndex >= 0 && m_selectedIndex < (int)m_userNames.size() && m_userNames[m_selectedIndex] != "admin");
        bool delHovered = canDelete && CheckCollisionPointRec(mousePos, delBtnRect);
        Color delCol = canDelete ? (delHovered ? Color{ 170, 50, 50, 255 } : Color{ 110, 35, 35, 255 }) : Color{ 60, 50, 50, 180 };
        DrawRectangleRec(delBtnRect, delCol);
        DrawRectangleLinesEx(delBtnRect, 2.0f, canDelete ? (delHovered ? WHITE : GOLD) : GRAY);
        m_fontButton.DrawTextCentered("DELETE", delBtnRect, 0.7f, canDelete ? WHITE : GRAY);

        // 4. [ CANCEL ]
        Rectangle cancelBtnRect = { 530.0f, 420.0f, 85.0f, 42.0f };
        bool cancelHovered = CheckCollisionPointRec(mousePos, cancelBtnRect);
        DrawRectangleRec(cancelBtnRect, cancelHovered ? Color{ 90, 80, 70, 255 } : Color{ 60, 50, 45, 255 });
        DrawRectangleLinesEx(cancelBtnRect, 2.0f, cancelHovered ? WHITE : GOLD);
        m_fontButton.DrawTextCentered("CANCEL", cancelBtnRect, 0.65f, WHITE);
    }
}
