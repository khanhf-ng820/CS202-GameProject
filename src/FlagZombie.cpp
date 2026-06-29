#include "FlagZombie.h"

FlagZombie::FlagZombie(Resources& res, float x, float y)
    : Zombie(res, x, y, 200, 25.0f, 100, "FlagZombie") {
    // Nạp reanim cho Flag Zombie
    // Tạm thời dùng Zombie.reanim, bạn có thể đổi đường dẫn nếu có file reanim riêng cho Flag Zombie
    getResources(res.GetAssetPath("assets/reanim/Zombie.reanim"));
    m_anim.SetBaseAnimation("anim_walk");

    // Hide other zombie accessories
    m_anim.SetTrackVisible("anim_bucket", false);
    // m_anim.SetTrackVisible("anim_cone", false); // Giữ lại nón (cone) theo yêu cầu
    m_anim.SetTrackVisible("anim_screendoor", false);
    m_anim.SetTrackVisible("Zombie_duckytube", false);
    m_anim.SetTrackVisible("Zombie_mustache", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor", false);
    m_anim.SetTrackVisible("Zombie_innerarm_screendoor_hand", false);
    m_anim.SetTrackVisible("Zombie_outerarm_screendoor", false);
    
    // Bàn tay cầm cờ (Zombie_flaghand) đang bị thiếu texture cán cờ/cờ nên trông như nắm đấm lơ lửng.
    // Ta sẽ ẩn nó đi và hiển thị lại bàn tay bình thường.
    m_anim.SetTrackVisible("Zombie_flaghand", false);
    // (Bỏ lệnh ẩn Zombie_outerarm_hand đi để nó hiện tay bình thường)
}

FlagZombie::~FlagZombie() {}

void FlagZombie::update(float deltaTime) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    // Ẩn đầu và nón khi chạy animation chết (vì trong game gốc cái đầu sẽ rớt ra thành một object riêng)
    if (currentAnim == "anim_death" || currentAnim == "anim_death2" || currentAnim == "anim_waterdeath") {
        m_anim.SetTrackVisible("anim_head1", false);
        m_anim.SetTrackVisible("anim_cone", false);
    } else {
        m_anim.SetTrackVisible("anim_head1", true);
        m_anim.SetTrackVisible("anim_cone", true);
    }

    if (!isDead()) {
        // Chỉ di chuyển khi đang ở animation walk
        if (currentAnim == "anim_walk" || currentAnim == "anim_walk2") {
            m_x -= m_speed * deltaTime;
        }
    }
}

void FlagZombie::draw() {
    m_anim.Draw(m_x, m_y, 1.6f); // Tham số scale 1.6f có thể điều chỉnh
}
