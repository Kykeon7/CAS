#pragma once

#include <SDL3/SDL.h>

#include <graphics/color.h>

namespace cas::platform {

/** Converts a graphics color to SDL's 8-bit color representation. */
inline SDL_Color to_sdl(const graphics::color& c) {
    return SDL_Color{
        static_cast<Uint8>(c.r * 255.0f),
        static_cast<Uint8>(c.g * 255.0f),
        static_cast<Uint8>(c.b * 255.0f),
        static_cast<Uint8>(c.a * 255.0f)
    };
}

/** Converts a graphics color to SDL's floating-point color representation. */
inline SDL_FColor to_sdl_fcolor(const graphics::color& c) {
    return SDL_FColor{
        c.r,
        c.g,
        c.b,
        c.a
    };
}

} // namespace cas::platform