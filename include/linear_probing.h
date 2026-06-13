#include "raylib.h"
#include <vector>
#include <string>
#include <vector>
#include <iostream>
#include "raymath.h"
#include "font.h"
#include "node.h"
#include "camera.h"
#include "operation.h"
#include "duration.h"
#include "menu.h"
#include "assets.h"
#include "highlight_code.h"
#include <fstream>
#pragma once

struct LinearProbingVisualizer {
    std::vector <std::vector <Node>> list;
    std::vector <std::vector <std::vector <Node>>> history;
    std::vector <int> snippets;
    const float smoothSpeed = 0.1f;
    float time_per_change = 0.5, progress = 0;
    int curent_state = 0;
    int split_point = 0;
    duration progress_duration, speed;
    camera cam;
    operation opp;
    std::string run();
    LinearProbingVisualizer() {
        list.resize(10, std::vector<Node>(1, {-1}));
    };
    void draw();
    void update();
    void insert(int value, int is_reset = 0);
    void delete_node(int value);
    bool search(int value);
    void DrawLinkedList(int id);
    void draw_task();
    void build_from_str(std::string str);   
    Vector2 get_pos(int i, int j);
    float get_alpha(int i, int j);
};