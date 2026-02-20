#include <core/input.h>

#include <cstring>

namespace cas {

Input& Input::instance() {
	static Input instance;
	return instance;
}

void Input::update() {
    SDL_PumpEvents();

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

    std::copy(
        m_curr_keys.begin(),
        m_curr_keys.end(),
        m_prev_keys.begin()
    );

    std::copy(
        keyboard_state,
        keyboard_state + SDL_SCANCODE_COUNT,
        m_curr_keys.begin()
    );

}

bool Input::is_key_held(SDL_Scancode key) const { return m_curr_keys[key]; }
bool Input::is_key_just_pressed(SDL_Scancode key) const { return m_curr_keys[key] && !m_prev_keys[key]; }
bool Input::is_key_just_released(SDL_Scancode key) const { return !m_curr_keys[key] && m_prev_keys[key]; }

} // namespace cas