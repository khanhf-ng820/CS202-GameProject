#include "raylib.h"
#include "resources.h"
#include "Reanimation.h"
#include "Projectile.h"
#include "SunItem.h"
#include <string>
#include <iostream>
#include <vector>
#pragma once

class Plant {
protected: 
    // Đổi thành protected để các class con như PeaShooter có thể truy cập tọa độ m_x, m_y và m_anim
    Resources& res; // Tham chiếu đến ResourceManager để vẽ cây (có thể là sprite hoặc animation)
    int m_x, m_y;          // Tọa độ trên ô lưới (Grid position) hoặc tọa độ pixel
    int m_hp;              // Máu hiện tại của cây
    int m_maxHp;           // Máu tối đa (để vẽ thanh máu nếu cần)
    int m_sunCost;         // Giá đặt cây (Ví dụ: Sunflower = 50, Peashooter = 100)
    float m_cooldownTime;  // Thời gian hồi chiêu để mua cây tiếp theo
    std::string m_name;    // Tên loại cây (để check logic hoặc debug)
    Reanimation m_anim;

public:
    // Constructor để khởi tạo các thông số cơ bản
    Plant(Resources& res, int x, int y, int hp, int sunCost, std::string name);
    
    virtual ~Plant() {}

    virtual void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) = 0;
    // Hàm vẽ cây lên màn hình (Mỗi cây có hình khác nhau nên cũng để virtual)
    virtual void draw() = 0;

    void getResources(std::string dir);
    // Các hàm phụ trợ (Getters / Setters) để các hệ thống khác tương tác
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getHp() const { return m_hp; }
    int getSunCost() const { return m_sunCost; }
    virtual bool isDead() const { return m_hp <= 0; }
    Reanimation& getAnim() { return m_anim; }
    
    // Hàm để Zombie gọi khi đang cắn cây này
    void takeDamage(int damage) { m_hp -= damage; }
};