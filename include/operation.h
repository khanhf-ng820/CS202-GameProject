#include "raylib.h"
#include <vector>
#include <string>
#include "textbox.h"
#include "font.h"
#include "textbox.h"
#include "tinyfiledialogs.h"
#include "assets.h"
#pragma once

struct operation {
    int x, y;
    std::vector <std::string> options, sub_options;
    text_box input;
    std::string command;
    int pop_up_app = 0, is_pending = false;
    std::string str_value;
    bool isabc = false;
    bool appear_sub_option = false;
    bool is_linked_list = false;
    std::vector <text_box> sub_option_boxes;
    std::string open_file_path();
    void initialize(int _x, int _y, bool _isabc = false);
    void update();
    void draw();
};