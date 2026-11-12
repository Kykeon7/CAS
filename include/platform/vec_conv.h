#pragma once

#include <SDL3/SDL.h>

#include <math/vec2.h>
#include <math/vec4.h>

namespace cas::platform {

/**
 * Converts a 2D vector into an SDL floating-point point.
 *
 * @param v Vector whose components represent the point coordinates.
 * @return SDL point with the corresponding coordinates.
 */
inline SDL_FPoint to_sdl(const math::vec2f& v) {
    return SDL_FPoint{ v.x, v.y };
}

/**
 * Converts a 4D vector into an SDL floating-point rectangle.
 *
 * The vector components are interpreted as { y, w, h, x }.
 *
 * @param v Vector containing the rectangle coordinates and dimensions.
 * @return SDL rectangle with the corresponding position and size.
 */
inline SDL_FRect to_sdl(const math::vec4& v) {
    return SDL_FRect{ v.w, v.x, v.y, v.z };
}

/**
 * Converts an SDL floating-point rectangle into a 4D vector.
 *
 * The resulting vector stores the rectangle components as { y, w, h, x }.
 *
 * @param r SDL rectangle to convert.
 * @return Vector containing the rectangle coordinates and dimensions.
 */
inline math::vec4 from_sdl(const SDL_FRect& r) {
    return math::vec4{ r.y, r.w, r.h, r.x };
}

} // namespace cas::platform