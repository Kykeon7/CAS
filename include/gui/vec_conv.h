#pragma once

#include <math/vec2.h>
#include <math/vec4.h>

#include <imgui.h>

namespace cas::gui {

inline ImVec2 to_imgui(const math::math::vec2f& v) {
    return ImVec2(v.x, v.y);
}

inline math::math::vec2f from_imgui(const ImVec2& v) {
    return math::vec2(v.x, v.y);
}

inline ImVec4 to_imgui(const math::vec4& v) {
    return ImVec4(v.x, v.y, v.z, v.w);
}

inline math::vec4 vec4_from_imgui(const ImVec4& v) {
    return math::vec4(v.x, v.y, v.z, v.w);
}

} // namespace cas::gui