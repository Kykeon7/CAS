#pragma once

#include <SDL3/SDL.h>

#include <inputs/input_keys.h>

namespace cas::inputs {

/** Converts an SDL scancode to the corresponding input key. */
constexpr key sdl_to_key(SDL_Scancode sc) {
    switch (sc) {
    case SDL_SCANCODE_W:      return key::W;
    case SDL_SCANCODE_A:      return key::A;
    case SDL_SCANCODE_S:      return key::S;
    case SDL_SCANCODE_D:      return key::D;
    case SDL_SCANCODE_SPACE:  return key::space;
    case SDL_SCANCODE_ESCAPE: return key::escape;
    case SDL_SCANCODE_R:      return key::R;
    case SDL_SCANCODE_Q:      return key::Q;
    case SDL_SCANCODE_F11:    return key::F11;
    case SDL_SCANCODE_LCTRL:  return key::left_ctrl;
    case SDL_SCANCODE_RCTRL:  return key::right_ctrl;
    case SDL_SCANCODE_LALT:   return key::left_alt;
    case SDL_SCANCODE_RALT:   return key::right_alt;
    case SDL_SCANCODE_LSHIFT: return key::left_shift;
    case SDL_SCANCODE_RSHIFT: return key::right_shift;
    default:                  return key::unknown;
    }
}

/** Converts an input key to the corresponding SDL scancode. */
constexpr SDL_Scancode key_to_sdl(key key) {
    switch (key) {
    case key::W:          return SDL_SCANCODE_W;
    case key::A:          return SDL_SCANCODE_A;
    case key::S:          return SDL_SCANCODE_S;
    case key::D:          return SDL_SCANCODE_D;
    case key::space:      return SDL_SCANCODE_SPACE;
    case key::escape:     return SDL_SCANCODE_ESCAPE;
    case key::R:          return SDL_SCANCODE_R;
    case key::Q:          return SDL_SCANCODE_Q;
    case key::F11:        return SDL_SCANCODE_F11;
    case key::left_ctrl:  return SDL_SCANCODE_LCTRL;
    case key::right_ctrl: return SDL_SCANCODE_RCTRL;
    case key::left_alt:   return SDL_SCANCODE_LALT;
    case key::right_alt:  return SDL_SCANCODE_RALT;
    case key::left_shift: return SDL_SCANCODE_LSHIFT;
    case key::right_shift:return SDL_SCANCODE_RSHIFT;
    default:              return SDL_SCANCODE_UNKNOWN;
    }
}

/** Converts a mouse button to its corresponding array index. */
constexpr int mouse_to_index(mouse_button btn) {
    switch (btn) {
    case mouse_button::left:   return 0;
    case mouse_button::right:  return 1;
    case mouse_button::middle: return 2;
    }

    return 0;
}

} // namespace cas::inputs