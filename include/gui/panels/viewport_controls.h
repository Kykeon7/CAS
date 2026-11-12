
#pragma once

#include <imgui.h>

#include <viewports/viewport.h>

namespace cas::gui::panels {

/**
 * @brief Renders viewport information and camera controls.
 *
 * @param viewport Viewport whose camera controls will be modified.
 */
inline void render_zoom_controls(cas::viewports::viewport& viewport) {
    constexpr float button_width = 32.0f;
    constexpr float button_height = 26.0f;
    constexpr float reset_zoom_width = button_width + 16.0f;
    constexpr float reset_position_width = 112.0f;

    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    const float controls_width =
        button_width * 2.0f +
        reset_zoom_width +
        reset_position_width +
        spacing * 3.0f;

    const float start_x = ImGui::GetCursorPosX();
    const float available_width = ImGui::GetContentRegionAvail().x;

    const auto& camera = viewport.camera();
    const auto& position = camera.position();
    const auto& size = camera.viewport_size();

    ImGui::AlignTextToFramePadding();

    ImGui::TextDisabled(
        "Zoom: %.0f%%  |  Camera: %.1f, %.1f  |  Size: %d x %d px",
        camera.zoom() * 100.0f,
        position.x,
        position.y,
        size.x,
        size.y
    );

    const float controls_x =
        start_x + available_width - controls_width;

    if (controls_x > ImGui::GetCursorPosX() + spacing)
        ImGui::SetCursorPosX(controls_x);
    else
        ImGui::NewLine();

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
        ImVec2(reset_position_width, button_height)
    )) {
        viewport.reset_position();
    }
}

} // namespace cas::gui::panels
