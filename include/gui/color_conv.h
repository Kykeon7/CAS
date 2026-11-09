#pragma once

#include <graphics/color.h>

#include <imgui.h>

namespace cas::gui {

inline ImVec4 to_imgui(const graphics::color& c) {
    return ImVec4(c.r, c.g, c.b, c.a);
}

inline graphics::color color_from_imgui(const ImVec4& v) {
    return graphics::color(v.x, v.y, v.z, v.w);
}

} // namespace cas::gui