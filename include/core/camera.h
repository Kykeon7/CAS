#pragma once
#include <core/math/vec2.h>

namespace cas {

class Camera {
public:
    Camera() = default;

    void set_position(const Vec2& pos);
    void move(const Vec2& delta);

    const Vec2& position() const;

    void set_zoom(float zoom);
    float zoom() const;

    Vec2 world_to_screen(const Vec2& world) const;
    Vec2 screen_to_world(const Vec2& screen) const;

private:
    Vec2 m_position{ 0.f, 0.f };
    float m_zoom{ 1.f };
};

}