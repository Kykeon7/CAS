#pragma once

#include <imgui.h>

#include <graphics/color.h>

namespace cas::gui {

/**
 * @brief Converts a graphics color to an ImGui color.
 *
 * @param color Color to convert.
 * @return Equivalent ImGui color.
 */
inline ImVec4 to_imgui(const graphics::color& color) {
    return ImVec4(color.r, color.g, color.b, color.a);
}

/**
 * @brief Converts an ImGui color to a graphics color.
 *
 * @param color Color to convert.
 * @return Equivalent graphics color.
 */
inline graphics::color color_from_imgui(const ImVec4& color) {
    return graphics::color(
        color.x,
        color.y,
        color.z,
        color.w
    );
}

} // namespace cas::gui