#include <raylib.h>
#include <string>
#include "font.h"
#include "assets.h"
#pragma once

struct text_box {
    Rectangle bounds;
    std::string text, num;
    bool isabc = false;
    bool appear = false;
    bool is_pending = false;
    std::string str_value;
    bool focused = false;
    double cursorTimer = 0.0;
    bool cursorVisible = true;
    int cursorIndex = 0;
    float scrollOffset = 0;
    bool accepting_long = false;
    int max_length = 0;
    void update();
    void draw();
    void initialize(int x, int y, int width, int height, std::string initialText, bool _accepting_long = false);
};