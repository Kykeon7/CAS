#include <core/input/input.h>
#include <core/input/key_conv.h>

#include <cstring>

namespace cas {

void Input::update() {
    SDL_PumpEvents();

    std::copy(m_curr_keys.begin(), m_curr_keys.end(), m_prev_keys.begin());

    const bool* keyboard = SDL_GetKeyboardState(nullptr);
    std::copy(keyboard, keyboard + SDL_SCANCODE_COUNT, m_curr_keys.begin());

    std::copy(m_curr_mouse.begin(), m_curr_mouse.end(), m_prev_mouse.begin());

    Uint32 buttons = SDL_GetMouseState(&m_mouse_pos.x, &m_mouse_pos.y);

    m_curr_mouse[0] = buttons & SDL_BUTTON_LMASK;
    m_curr_mouse[1] = buttons & SDL_BUTTON_RMASK;
    m_curr_mouse[2] = buttons & SDL_BUTTON_MMASK;

    m_mouse_delta = m_mouse_pos - m_prev_mouse_pos;
    m_prev_mouse_pos = m_mouse_pos;

    m_mouse_wheel = 0.0f;
}

bool Input::is_key_held(Key key) const { 
    auto sc = key_to_sdl(key);
    return m_curr_keys[sc]; 
}
bool Input::is_key_pressed(Key key) const { 
    auto sc = key_to_sdl(key);
    return m_curr_keys[sc] && !m_prev_keys[sc]; 
}
bool Input::is_key_released(Key key) const { 
    auto sc = key_to_sdl(key);
    return !m_curr_keys[sc] && m_prev_keys[sc];
}
bool Input::is_mouse_held(MouseButton btn) const { 
    auto idx = mouse_to_index(btn);
    return m_curr_mouse[idx];
}
bool Input::is_mouse_pressed(MouseButton btn) const { 
    auto idx = mouse_to_index(btn);
    return m_curr_mouse[idx] && !m_prev_mouse[idx];
}

bool Input::is_mouse_released(MouseButton btn) const {
    auto idx = mouse_to_index(btn);
    return !m_curr_mouse[idx] && m_prev_mouse[idx];
}

Vec2 Input::mouse_position() const{ return m_mouse_pos; }
Vec2 Input::mouse_delta() const { return m_mouse_delta; }
float Input::mouse_wheel() const { return m_mouse_wheel; }
void Input::add_mouse_wheel(float w) { m_mouse_wheel += w; }

} // namespace cas