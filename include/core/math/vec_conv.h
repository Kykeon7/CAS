#pragma once

#include "vec2.h"
#include "vec4.h"

#include <SDL3/SDL.h>
#include <imgui.h>

namespace cas {

inline ImVec2 to_imgui(const Vec2& v) {
    return ImVec2(v.x, v.y);
}

inline Vec2 from_imgui(const ImVec2& v) {
    return Vec2(v.x, v.y);
}

inline SDL_FPoint to_sdl(const Vec2& v) {
    return SDL_FPoint{ v.x, v.y };
}

inline ImVec4 to_imgui(const Vec4& v) {
    return ImVec4(v.x, v.y, v.z, v.w);
}

inline Vec4 vec4_from_imgui(const ImVec4& v) {
    return Vec4(v.x, v.y, v.z, v.w);
}

}