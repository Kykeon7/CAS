#pragma once

#include <imgui.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <viewports/manager.h>

namespace cas::gui::panels {

/**
 * @brief Renders and manages the viewport tabs.
 *
 * Supports selecting, creating, closing, and renaming viewports.
 *
 * @param viewports Viewport manager to operate on.
 */
inline void render_viewport_tabs(cas::viewports::manager& viewports) {
    static bool show_last_viewport_error = false;
    static cas::viewports::manager::viewport_id editing_id =
        cas::viewports::manager::max_viewports;
    static char name_buffer[128]{};
    static bool focus_name_input = false;
    static ImVec2 editing_min{};
    static ImVec2 editing_max{};

    std::vector<cas::viewports::manager::viewport_id> to_close;

    if (ImGui::BeginTabBar("ViewportsTabs")) {
        for (
            cas::viewports::manager::viewport_id id = 0;
            id < cas::viewports::manager::max_viewports;
            ++id
        ) {
            if (!viewports.exists(id))
                continue;

            auto& active_viewport = viewports.get(id);

            const std::string label =
                active_viewport.name() + "###viewport_tab_" +
                std::to_string(id);

            bool open = true;

            const bool selected =
                ImGui::BeginTabItem(label.c_str(), &open);

            const bool hovered = ImGui::IsItemHovered();

            if (id == editing_id) {
                editing_min = ImGui::GetItemRectMin();
                editing_max = ImGui::GetItemRectMax();
            }

            if (
                hovered &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
            ) {
                editing_id = id;

                const std::string& name = active_viewport.name();
                const std::size_t length = std::min(
                    name.size(),
                    sizeof(name_buffer) - 1
                );

                std::memcpy(name_buffer, name.data(), length);
                name_buffer[length] = '\0';

                editing_min = ImGui::GetItemRectMin();
                editing_max = ImGui::GetItemRectMax();
                focus_name_input = true;
            }

            if (selected) {
                if (viewports.active_id() != id)
                    viewports.set_active(id);

                ImGui::EndTabItem();
            }

            if (!open)
                to_close.push_back(id);
        }

        ImGui::BeginDisabled(
            viewports.count() >= cas::viewports::manager::max_viewports
        );

        if (ImGui::TabItemButton("+"))
            viewports.create();

        ImGui::EndDisabled();
        ImGui::EndTabBar();
    }

    if (
        editing_id < cas::viewports::manager::max_viewports &&
        viewports.exists(editing_id)
    ) {
        ImGui::SetNextWindowPos(editing_min);
        ImGui::SetNextWindowSize(
            ImVec2(
                editing_max.x - editing_min.x,
                editing_max.y - editing_min.y
            )
        );

        ImGui::SetNextWindowBgAlpha(1.0f);

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowFocus();

        if (ImGui::Begin(
            "##viewport_name_editor",
            nullptr,
            flags
        )) {
            if (focus_name_input) {
                ImGui::SetKeyboardFocusHere();
                focus_name_input = false;
            }

            ImGui::SetNextItemWidth(-1.0f);

            const bool confirmed = ImGui::InputText(
                "##viewport_name",
                name_buffer,
                sizeof(name_buffer),
                ImGuiInputTextFlags_EnterReturnsTrue |
                ImGuiInputTextFlags_AutoSelectAll
            );

            const bool cancelled =
                ImGui::IsKeyPressed(ImGuiKey_Escape);

            if (confirmed) {
                if (name_buffer[0] != '\0')
                    viewports.get(editing_id).set_name(name_buffer);

                editing_id = cas::viewports::manager::max_viewports;
            }
            else if (cancelled) {
                editing_id = cas::viewports::manager::max_viewports;
            }
        }

        ImGui::End();
    }

    std::sort(to_close.rbegin(), to_close.rend());

    for (const auto id : to_close) {
        if (viewports.count() <= 1) {
            show_last_viewport_error = true;
            continue;
        }

        if (editing_id == id)
            editing_id = cas::viewports::manager::max_viewports;

        viewports.destroy(id);
    }

    if (show_last_viewport_error) {
        ImGui::OpenPopup("Cannot close viewport");
        show_last_viewport_error = false;
    }

    ImGui::SetNextWindowSize(
        ImVec2(420.0f, 0.0f),
        ImGuiCond_Appearing
    );

    if (ImGui::BeginPopupModal(
        "Cannot close viewport",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
    )) {
        ImGui::Text("The last viewport cannot be closed.");
        ImGui::Text("At least one viewport must remain open.");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        constexpr float button_width = 120.0f;

        const float available_width =
            ImGui::GetContentRegionAvail().x;

        ImGui::SetCursorPosX(
            ImGui::GetCursorPosX() +
            (available_width - button_width) * 0.5f
        );

        if (ImGui::Button("OK", ImVec2(button_width, 0.0f)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

} // namespace cas::gui::panels