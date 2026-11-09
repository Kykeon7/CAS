
#pragma once

#include <viewports/viewport.h>
#include <imgui.h>

namespace cas::gui::panels {

inline void render_zoom_controls(cas::viewports::viewport& viewport) {
    const float button_width = 32.0f;
    const float button_height = 26.0f;
    const float reset_zoom_width = button_width + 16.0f;
    const float reset_position_width = 112.0f;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    const float controls_width =
        button_width * 2.0f +
        reset_zoom_width +
        reset_position_width +
        spacing * 3.0f;

    const float available_width =
        ImGui::GetContentRegionAvail().x;

    ImGui::SetCursorPosX(
        ImGui::GetCursorPosX() +
        available_width - controls_width
    );

    if (ImGui::Button(
        "-##zoom_out",
        ImVec2(button_width, button_height)
    )) {
        viewport.zoom_out();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "+##zoom_in",
        ImVec2(button_width, button_height)
    )) {
        viewport.zoom_in();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "1:1##zoom_reset",
        ImVec2(reset_zoom_width, button_height)
    )) {
        viewport.zoom_reset();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "(0,0)##reset_position",
        ImVec2(reset_zoom_width, button_height)
    )) {
        viewport.reset_position();
    }
}

} // namespace cas::gui::panels
