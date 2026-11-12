#include <inputs/input.h>

#include <algorithm>

#include <inputs/key_conv.h>

namespace cas::inputs {

void input::update() {
    SDL_PumpEvents();

    std::copy(_curr_keys.begin(), _curr_keys.end(), _prev_keys.begin());

    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    std::copy(
        keyboard,
        keyboard + SDL_SCANCODE_COUNT,
        _curr_keys.begin()
    );

    std::copy(_curr_mouse.begin(), _curr_mouse.end(), _prev_mouse.begin());

    const Uint32 buttons = SDL_GetMouseState(
        &_mouse_pos.x,
        &_mouse_pos.y
    );

    _curr_mouse[0] = (buttons & SDL_BUTTON_LMASK) != 0;
    _curr_mouse[1] = (buttons & SDL_BUTTON_RMASK) != 0;
    _curr_mouse[2] = (buttons & SDL_BUTTON_MMASK) != 0;

    _mouse_delta = _mouse_pos - _prev_mouse_pos;
    _prev_mouse_pos = _mouse_pos;

    _mouse_wheel = 0.0f;
}

bool input::is_key_held(key key) const {
    const auto sc = key_to_sdl(key);
    return _curr_keys[sc];
}

bool input::is_key_pressed(key key) const {
    const auto sc = key_to_sdl(key);
    return _curr_keys[sc] && !_prev_keys[sc];
}

bool input::is_key_released(key key) const {
    const auto sc = key_to_sdl(key);
    return !_curr_keys[sc] && _prev_keys[sc];
}

bool input::is_mouse_held(mouse_button btn) const {
    const auto idx = mouse_to_index(btn);
    return _curr_mouse[idx];
}

bool input::is_mouse_pressed(mouse_button btn) const {
    const auto idx = mouse_to_index(btn);
    return _curr_mouse[idx] && !_prev_mouse[idx];
}

bool input::is_mouse_released(mouse_button btn) const {
    const auto idx = mouse_to_index(btn);
    return !_curr_mouse[idx] && _prev_mouse[idx];
}

math::vec2f input::mouse_position() const {
    return _mouse_pos;
}

math::vec2f input::mouse_delta() const {
    return _mouse_delta;
}

float input::mouse_wheel() const {
    return _mouse_wheel;
}

void input::add_mouse_wheel(float w) {
    _mouse_wheel += w;
}

} // namespace cas