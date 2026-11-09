
#include <gui/panels/appearance.h>

#include <gui/color_conv.h>

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <imgui_internal.h>

namespace cas::gui::panels {

appearance::appearance(viewport_view& view)
    : _view(view)
{
}

void appearance::render() {
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

void appearance::render_colors() {
    ImGui::Text("Colors");

    auto& preview_config = _view.preview_config();

    ImVec4 color = gui::to_imgui(preview_config.alive_color);

    if (ImGui::ColorEdit3("Alive", &color.x)) 
        preview_config.alive_color = gui::color_from_imgui(color);

    color = gui::to_imgui(preview_config.grid_color);

    if (ImGui::ColorEdit3("Grid", &color.x)) 
        preview_config.grid_color = gui::color_from_imgui(color);
    
    color = gui::to_imgui(preview_config.background_color);

    if (ImGui::ColorEdit3("Background", &color.x)) 
        preview_config.background_color = gui::color_from_imgui(color);
    
    color = gui::to_imgui(preview_config.persistent_color);

    if (ImGui::ColorEdit3("Persistent Cell", &color.x)) 
        preview_config.persistent_color = gui::color_from_imgui(color);
}

void appearance::render_layout() {
    auto& preview_config = _view.preview_config();

    ImGui::Text("Layout");

    ImGui::SliderFloat(
        "Cell Size",
        &preview_config.cell_size,
        1.0f,
        50.0f
    );

    ImGui::DragFloat2(
        "Offset",
        &preview_config.offset.x,
        1.0f
    );
}

void appearance::render_actions() {
    const float button_width = ImGui::GetContentRegionAvail().x;

    if (ImGui::Button("Apply", ImVec2(button_width, 0.0f))) {
        _view.config() = _view.preview_config();
    }

    if (ImGui::Button("Reset", ImVec2(button_width, 0.0f))) {
        _view.preview_config() = _view.config();
    }

    if (ImGui::Button("Default", ImVec2(button_width, 0.0f))) {
        _view.preview_config() = cas::rendering::automaton::config{};
    }
}

void appearance::render_preview() const {
    const auto& preview_config = _view.preview_config();

    ImGui::Text("Preview");

    ImVec2 visible_size = ImGui::GetContentRegionAvail();

    ImVec2 dummy_size = visible_size;
    dummy_size.x *= 2.0f;
    dummy_size.y = (std::min)(dummy_size.y, 120.0f);

    const ImVec2 pos = ImGui::GetCursorScreenPos();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    // Limitar el dibujo al área visible del preview.
    draw->PushClipRect(
        pos,
        {
            pos.x + visible_size.x,
            pos.y + dummy_size.y
        },
        true
    );

    const ImU32 background = ImGui::ColorConvertFloat4ToU32(
        gui::to_imgui(preview_config.background_color)
    );

    const ImU32 grid = ImGui::ColorConvertFloat4ToU32(
        gui::to_imgui(preview_config.grid_color)
    );

    const ImU32 alive = ImGui::ColorConvertFloat4ToU32(
        gui::to_imgui(preview_config.alive_color)
    );

    const ImU32 persistent = ImGui::ColorConvertFloat4ToU32(
        gui::to_imgui(preview_config.persistent_color)
    );

    draw->AddRectFilled(
        pos,
        {
            pos.x + visible_size.x,
            pos.y + dummy_size.y
        },
        background
    );

    const float radius = preview_config.cell_size;
    const float hex_w = radius * 2.0f;
    const float hex_h = std::sqrt(3.0f) * radius;

    // Espaciado regular para hexágonos flat-top.
    const float spacing_x = hex_w * 0.75f;
    const float spacing_y = hex_h;

    auto draw_hex = [&](ImVec2 center, ImU32 color, bool filled) {
        ImVec2 points[6];

        for (int i = 0; i < 6; ++i) {
            const float angle = IM_PI / 3.0f * i;

            points[i] = {
                center.x + std::cos(angle) * radius,
                center.y + std::sin(angle) * radius
            };
        }

        if (filled) {
            draw->AddConvexPolyFilled(
                points,
                6,
                color
            );
        }

        draw->AddPolyline(
            points,
            6,
            grid,
            ImDrawFlags_Closed,
            1.0f
        );
        };

    // Extensión de la cuadrícula alrededor del centro visible.
    const int q_extent =
        static_cast<int>(
            std::ceil(visible_size.x / (2.0f * spacing_x))
            ) + 2;

    const int r_extent =
        static_cast<int>(
            std::ceil(visible_size.y / (2.0f * spacing_y))
            ) + q_extent / 2 + 2;

    const ImVec2 center = {
        pos.x + visible_size.x * 0.5f,
        pos.y + dummy_size.y * 0.5f
    };

    // Generar la cuadrícula usando coordenadas axiales.
    for (int q = -q_extent; q <= q_extent; ++q) {
        for (int r = -r_extent; r <= r_extent; ++r) {
            const float x = center.x + q * spacing_x;

            const float y =
                center.y + (r + q * 0.5f) * spacing_y;

            // Distancia hexagonal al centro.
            const int distance = (std::max)({
                std::abs(q),
                std::abs(r),
                std::abs(q + r)
                });

            const bool persistent_cell = distance <= 1;
            const bool alive_cell = distance <= 3;

            if (persistent_cell) {
                draw_hex({ x, y }, persistent, true);
            }
            else if (alive_cell) {
                draw_hex({ x, y }, alive, true);
            }
            else {
                draw_hex({ x, y }, grid, false);
            }
        }
    }

    draw->PopClipRect();

    ImGui::Dummy(dummy_size);
}

} // namespace cas::gui::panels