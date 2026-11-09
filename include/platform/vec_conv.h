#pragma once

#include <math/vec2.h>
#include <math/vec4.h>

#include <SDL3/SDL.h>

namespace cas::platform {

inline SDL_FPoint to_sdl(const math::vec2f& v) {
    return SDL_FPoint{ v.x, v.y };
}

inline SDL_FRect to_sdl(const math::vec4& v) {
    return SDL_FRect{ v.w, v.x, v.y, v.z };
}

inline math::vec4 from_sdl(const SDL_FRect& r) {
    return math::vec4{ r.y, r.w, r.h, r.x };
}

} // namespace cas::platform