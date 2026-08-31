#pragma once

#include "color.h"

#include <SDL3/SDL.h>
#include <imgui.h>

namespace cas {

inline SDL_Color to_sdl(const color& c) {
    return SDL_Color{
        static_cast<Uint8>(c.r * 255.0f),
        static_cast<Uint8>(c.g * 255.0f),
        static_cast<Uint8>(c.b * 255.0f),
        static_cast<Uint8>(c.a * 255.0f)
    };
}

inline SDL_FColor to_sdl_fcolor(const color& c) {
    return SDL_FColor{
        c.r,
        c.g,
        c.b,
        c.a
    };
}

inline ImVec4 to_imgui(const color& c) {
    return ImVec4(c.r, c.g, c.b, c.a);
}

inline color color_from_imgui(const ImVec4& v) {
    return color(v.x, v.y, v.z, v.w);
}

} // namespace cas