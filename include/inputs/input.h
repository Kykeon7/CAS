#pragma once

#include <SDL3/SDL.h>
#include <array>

#include <math/vec2.h>
#include <inputs/input_keys.h>

namespace cas::inputs {

class input {
public:
    void update();

    bool is_key_held(key key) const;
    bool is_key_pressed(key key) const;
    bool is_key_released(key key) const;

    bool is_mouse_held(mouse_button btn) const;
    bool is_mouse_pressed(mouse_button btn) const;
    bool is_mouse_released(mouse_button btn) const;

    math::vec2f mouse_position() const;
    math::vec2f mouse_delta() const;
    float mouse_wheel() const;
    void add_mouse_wheel(float w);

private:
    std::array<bool, SDL_SCANCODE_COUNT> _prev_keys{};
    std::array<bool, SDL_SCANCODE_COUNT> m_curr_keys{};

    std::array<bool, 3> _prev_mouse{};
    std::array<bool, 3> _curr_mouse{};

    math::vec2f _mouse_pos{};
    math::vec2f _prev_mouse_pos{};
    math::vec2f _mouse_delta{};

    float _mouse_wheel = 0.0f;
};

} // namespace cas::inputs