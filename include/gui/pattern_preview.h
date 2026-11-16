#pragma once

#include <imgui.h>

#include <algorithm>
#include <limits>
#include <cmath>

#include <automata/patterns.h>
#include <cmath>

#include <automata/patterns.h>

#include <automata/pattern.h>

namespace cas::gui {

/// @brief Draws a pointy-top hexagon matching the viewport hex layout.
inline void draw_pattern_hex(
    ImDrawList* draw, ImVec2 center, float radius, ImU32 fill,
    ImU32 outline = IM_COL32(125, 135, 150, 95)
) {
    constexpr float sqrt3half = 0.8660254037844386f;
    const ImVec2 points[6] = {
        {center.x, center.y - radius},
        {center.x + sqrt3half * radius, center.y - radius * 0.5f},
        {center.x + sqrt3half * radius, center.y + radius * 0.5f},
        {center.x, center.y + radius},
        {center.x - sqrt3half * radius, center.y + radius * 0.5f},
        {center.x - sqrt3half * radius, center.y - radius * 0.5f}
    };
    draw->AddConvexPolyFilled(points, 6, fill);
    draw->AddPolyline(points, 6, outline, ImDrawFlags_Closed, 1.0f);
}

/// @brief Draws the exact cell states in a miniature square or hexagonal grid.
inline void draw_pattern_preview(
    ImDrawList* draw,
    const automata::pattern& source,
    const ImVec2& origin,
    const ImVec2& size,
    bool hexagonal = false,
    ImU32 state1 = IM_COL32(65, 170, 235, 255),
    ImU32 state2 = IM_COL32(245, 180, 65, 255)
) {
    if (source.cells().empty() || size.x <= 0.0f || size.y <= 0.0f)
        return;

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    constexpr float sqrt3 = 1.7320508075688772f;

    for (const auto& cell : source.cells()) {
        const float x = hexagonal ? sqrt3 * (cell.offset.x + cell.offset.y * 0.5f) :
            static_cast<float>(cell.offset.x);
        const float y = hexagonal ? 1.5f * cell.offset.y :
            static_cast<float>(cell.offset.y);
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
    }
    const float half_width = hexagonal ? sqrt3 * 0.5f : 0.5f;
    const float half_height = hexagonal ? 1.0f : 0.5f;
    const float scale = std::min(size.x / (max_x - min_x + 2.0f * half_width),
                                 size.y / (max_y - min_y + 2.0f * half_height));
    const float cx = origin.x + size.x * 0.5f - (min_x + max_x) * 0.5f * scale;
    const float cy = origin.y + size.y * 0.5f - (min_y + max_y) * 0.5f * scale;

    for (const auto& cell : source.cells()) {
        const float x = hexagonal ? sqrt3 * (cell.offset.x + cell.offset.y * 0.5f) :
            static_cast<float>(cell.offset.x);
        const float y = hexagonal ? 1.5f * cell.offset.y :
            static_cast<float>(cell.offset.y);
        const ImVec2 center{cx + x * scale, cy + y * scale};
        const ImU32 fill = cell.state == 2 ? state2 : state1;
        if (hexagonal)
            draw_pattern_hex(draw, center, scale * 0.94f, fill);
        else {
            const float inset = std::min(2.0f, scale * 0.12f);
            draw->AddRectFilled(
                {center.x - scale * 0.5f + inset, center.y - scale * 0.5f + inset},
                {center.x + scale * 0.5f - inset, center.y + scale * 0.5f - inset}, fill
            );
        }
    }
}

} // namespace cas::gui
