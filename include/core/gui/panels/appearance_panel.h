#pragma once

#include <core/gui/panels/gui_panel.h>
#include <core/graphics/color.h>
#include <core/rendering/automaton_renderers/render_config.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <core/graphics/color_conv.h>

namespace cas::panels {

    class Appearance : public Panel {
    public:
        Appearance(AutomatonRenderConfig& config)
            : m_config(config)
            , m_preview_config(config)
        {
        }

        void render() override {
            ImGui::Text("Appearance");

            ImGui::PushItemWidth(-1);

            ImGui::BeginChild(
                "AppearanceContent",
                ImGui::GetContentRegionAvail()
            );

            render_colors();
            ImGui::Separator();

            render_layout();
            ImGui::Separator();

            render_preview();
            ImGui::Separator();

            render_actions();

            ImGui::EndChild();

            ImGui::PopItemWidth();
        }

    private:

        void render_colors() {
            ImGui::Text("Colors");

            ImVec4 color = to_imgui(m_preview_config.alive_color);

            if (ImGui::ColorEdit3("Alive", (float*)&color))
                m_preview_config.alive_color = color_from_imgui(color);

            color = to_imgui(m_preview_config.grid_color);

            if (ImGui::ColorEdit3("Grid", (float*)&color))
                m_preview_config.grid_color = color_from_imgui(color);

            color = to_imgui(m_preview_config.background_color);

            if (ImGui::ColorEdit3("Background", (float*)&color))
                m_preview_config.background_color = color_from_imgui(color);
        }

        void render_layout() {
            ImGui::Text("Layout");

            ImGui::SliderFloat(
                "Cell Size",
                &m_preview_config.cell_size,
                1.0f,
                50.0f
            );

            ImGui::DragFloat2(
                "Offset",
                &m_preview_config.offset.x,
                1.0f
            );
        }

        void render_actions() {

            if (ImGui::Button("Apply")) {
                m_config = m_preview_config;
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset")) {
                m_preview_config = m_config;
            }
        }

        void render_preview() {
            ImGui::Text("Preview");

            ImVec2 size = ImGui::GetContentRegionAvail();
            size.y = (std::min)(size.y, 120.f);

            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();

            ImU32 bg = ImGui::ColorConvertFloat4ToU32(
                to_imgui(m_preview_config.background_color)
            );

            ImU32 grid = ImGui::ColorConvertFloat4ToU32(
                to_imgui(m_preview_config.grid_color)
            );

            ImU32 alive = ImGui::ColorConvertFloat4ToU32(
                to_imgui(m_preview_config.alive_color)
            );

            draw->AddRectFilled(
                pos,
                { pos.x + size.x, pos.y + size.y },
                bg
            );

            constexpr int cols = 4;
            constexpr int rows = 4;

            float radius = m_preview_config.cell_size;

            float hex_w = radius * 2.0f;
            float hex_h = std::sqrt(3.0f) * radius;

            float spacing_x = hex_w * 0.75f;
            float spacing_y = hex_h;

            auto draw_hex = [&](ImVec2 center, bool filled) {

                ImVec2 pts[6];

                for (int i = 0; i < 6; ++i) {

                    float angle = IM_PI / 3.0f * i;

                    pts[i] = {
                        center.x + std::cos(angle) * radius,
                        center.y + std::sin(angle) * radius
                    };
                }

                if (filled)
                    draw->AddConvexPolyFilled(
                        pts,
                        6,
                        alive
                    );

                draw->AddPolyline(
                    pts,
                    6,
                    grid,
                    ImDrawFlags_Closed,
                    1.0f
                );
                };

            for (int r = 0; r < rows; ++r) {
                for (int q = 0; q < cols; ++q) {

                    float x = pos.x + 20.0f + q * spacing_x;

                    if (r % 2 == 1)
                        x += spacing_x * 0.5f;

                    float y = pos.y + 20.0f + r * (spacing_y * 0.85f);

                    bool alive_cell =
                        (q == 1 && r == 1) ||
                        (q == 2 && r == 2);

                    draw_hex({ x, y }, alive_cell);
                }
            }

            ImGui::Dummy(size);
        }

    private:
        AutomatonRenderConfig& m_config;
        AutomatonRenderConfig m_preview_config;
    };

} // namespace cas::panels