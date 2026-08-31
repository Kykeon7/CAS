#pragma once

#include <SDL3/SDL.h>
#include <core/input/input_keys.h>

namespace cas {

static constexpr Key sdl_to_key(SDL_Scancode sc) {
    switch (sc) {
    case SDL_SCANCODE_W: return Key::W;
    case SDL_SCANCODE_A: return Key::A;
    case SDL_SCANCODE_S: return Key::S;
    case SDL_SCANCODE_D: return Key::D;
    case SDL_SCANCODE_SPACE: return Key::Space;
    case SDL_SCANCODE_ESCAPE: return Key::Escape;
    case SDL_SCANCODE_R: return Key::R;
    case SDL_SCANCODE_F11: return Key::F11;
    default: return Key::Unknown;
    }
}   

static constexpr SDL_Scancode key_to_sdl(Key key) {
    switch (key) {
    case Key::W:       return SDL_SCANCODE_W;
    case Key::A:       return SDL_SCANCODE_A;
    case Key::S:       return SDL_SCANCODE_S;
    case Key::D:       return SDL_SCANCODE_D;
    case Key::Space:   return SDL_SCANCODE_SPACE;
    case Key::Escape:  return SDL_SCANCODE_ESCAPE;
    case Key::R:       return SDL_SCANCODE_R;
    case Key::F11:     return SDL_SCANCODE_F11;
    default:           return SDL_SCANCODE_UNKNOWN;
    }
}

static constexpr int mouse_to_index(MouseButton btn) {
    switch (btn) {
    case MouseButton::Left:   return 0;
    case MouseButton::Right:  return 1;
    case MouseButton::Middle: return 2;
    }
    return 0; // fallback
}

} // namespace cas