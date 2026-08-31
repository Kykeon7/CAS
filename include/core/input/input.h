#pragma once

#include <SDL3/SDL.h>
#include <array>

#include <core/math/vec2.h>
#include <core/input/input_keys.h>

namespace cas {

class Input {
public:
    void update();

    bool is_key_held(Key key) const;
    bool is_key_pressed(Key key) const;
    bool is_key_released(Key key) const;

    bool is_mouse_held(MouseButton btn) const;
    bool is_mouse_pressed(MouseButton btn) const;
    bool is_mouse_released(MouseButton btn) const;

    Vec2 mouse_position() const;
    Vec2 mouse_delta() const;
    float mouse_wheel() const;
    void add_mouse_wheel(float w);

private:
    std::array<bool, SDL_SCANCODE_COUNT> m_prev_keys{};
    std::array<bool, SDL_SCANCODE_COUNT> m_curr_keys{};

    std::array<bool, 3> m_prev_mouse{};
    std::array<bool, 3> m_curr_mouse{};

    Vec2 m_mouse_pos{};
    Vec2 m_prev_mouse_pos{};
    Vec2 m_mouse_delta{};

    float m_mouse_wheel = 0.0f;
};

} // namespace cas