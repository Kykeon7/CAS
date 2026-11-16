
#include <gui/panels/viewport.h>

#include <imgui.h>

#include <cmath>

#include <automata/pattern_tests.h>
#include <automata/patterns.h>
#include <automata/rules/factory.h>
#include <gui/pattern_preview.h>
#include <gui/panels/viewport_controls.h>
#include <gui/panels/viewport_tabs.h>
#include <system/profiler.h>

namespace cas::gui::panels {

viewport::viewport(
    cas::viewports::manager& viewports,
    const inputs::input& input
) :
    _viewports(viewports),
    _input(input)
{
}

void viewport::render() {
    render_viewport_tabs(_viewports);

    auto& active_viewport = _viewports.active_viewport();

    // Controles superiores.
    render_zoom_controls(active_viewport);

    // Área de renderizado.
    const ImVec2 available = ImGui::GetContentRegionAvail();

    if (available.x <= 0.0f || available.y <= 0.0f)
        return;

    const ImVec2 viewport_pos = ImGui::GetCursorScreenPos();

    active_viewport.set_size({
        static_cast<int>(available.x),
        static_cast<int>(available.y)
    });

    const bool is_hovered = ImGui::IsWindowHovered();

    active_viewport.handle_input(
        _input,
        is_hovered,
        {
            viewport_pos.x,
            viewport_pos.y
        }
    );

    auto& profiler = system::profiler::instance();

    profiler.begin(system::profile_id::render);

    if (auto texture = active_viewport.render()) {
        ImGui::Image(
            static_cast<ImTextureID>(texture->get().imgui_id()),
            available
        );

        render_pattern_preview(
            active_viewport,
            viewport_pos,
            available
        );

        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(
                "CAS_PATTERN",
                ImGuiDragDropFlags_AcceptNoDrawDefaultRect
            );

            if (payload &&
                payload->IsDelivery() &&
                payload->DataSize == sizeof(int)) {
                const int index =
                    *static_cast<const int*>(payload->Data);

                if (index >= 0 &&
                    index < static_cast<int>(
                        automata::patterns::catalogue.size()
                    )) {
                    auto& automaton =
                        active_viewport.simulation().automaton();

                    const auto& config = active_viewport.config();

                    if (automata::patterns::supports(
                            automata::patterns::catalogue[index].identifier,
                            automaton.topology(),
                            automata::rules::rule_to_string(automaton.rule())
                        ) &&
                        config.cell_size > 0.0f) {
                        const ImVec2 mouse = ImGui::GetMousePos();

                        const auto world =
                            active_viewport.screen_to_world({
                                mouse.x - viewport_pos.x,
                                mouse.y - viewport_pos.y
                            });

                        int x = 0;
                        int y = 0;
                        if (automaton.topology().is_moore()) {
                            x = static_cast<int>(std::floor(
                                (world.x - config.offset.x) / config.cell_size +
                                automaton.width() * 0.5f + 0.5f));
                            y = static_cast<int>(std::floor(
                                (world.y - config.offset.y) / config.cell_size +
                                automaton.height() * 0.5f + 0.5f));
                        }
                        else {
                            const auto [q, r] = active_viewport.layout().world_to_hex(
                                world.x, world.y, config.cell_size,
                                config.offset.x, config.offset.y);
                            x = q + automaton.width() / 2;
                            y = r + automaton.height() / 2;
                        }

                        const auto source = automata::patterns::create(
                            automata::patterns::catalogue[index].identifier
                        );

                        (void)automata::stamp_pattern(
                            automaton,
                            source,
                            x,
                            y
                        );
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }
    }

    profiler.end(system::profile_id::render);
}

void viewport::render_pattern_preview(
    cas::viewports::viewport& active_viewport,
    const ImVec2& viewport_pos,
    const ImVec2& available
) {
    const ImGuiPayload* dragging = ImGui::GetDragDropPayload();

    if (!dragging ||
        !dragging->IsDataType("CAS_PATTERN") ||
        dragging->DataSize != sizeof(int))
        return;

    const ImVec2 mouse = ImGui::GetMousePos();

    if (!ImGui::IsMouseHoveringRect(
        viewport_pos,
        {
            viewport_pos.x + available.x,
            viewport_pos.y + available.y
        }
    ))
        return;

    const int index =
        *static_cast<const int*>(dragging->Data);

    if (index < 0 ||
        index >= static_cast<int>(
            automata::patterns::catalogue.size()
        ))
        return;

    auto& automaton =
        active_viewport.simulation().automaton();

    const auto& config = active_viewport.config();

    if (!automata::patterns::supports(
            automata::patterns::catalogue[index].identifier,
            automaton.topology(),
            automata::rules::rule_to_string(automaton.rule())
        ) || config.cell_size <= 0.0f)
        return;

    const auto world = active_viewport.screen_to_world({
        mouse.x - viewport_pos.x,
        mouse.y - viewport_pos.y
    });

    int x = 0;
    int y = 0;
    if (automaton.topology().is_moore()) {
        x = static_cast<int>(std::floor(
            (world.x - config.offset.x) / config.cell_size +
            automaton.width() * 0.5f + 0.5f));
        y = static_cast<int>(std::floor(
            (world.y - config.offset.y) / config.cell_size +
            automaton.height() * 0.5f + 0.5f));
    }
    else {
        const auto [q, r] = active_viewport.layout().world_to_hex(
            world.x, world.y, config.cell_size,
            config.offset.x, config.offset.y);
        x = q + automaton.width() / 2;
        y = r + automaton.height() / 2;
    }

    const auto source = automata::patterns::create(
        automata::patterns::catalogue[index].identifier
    );

    bool valid = true;

    for (const auto& cell : source.cells()) {
        if (!automaton.in_bounds(
            x + cell.offset.x,
            y + cell.offset.y
        ) || cell.state >= automaton.rule().state_count()) {
            valid = false;
            break;
        }
    }

    const float side =
        config.cell_size * active_viewport.camera().zoom();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->PushClipRect(
        viewport_pos,
        {
            viewport_pos.x + available.x,
            viewport_pos.y + available.y
        },
        true
    );

    const ImU32 fill = valid ?
        IM_COL32(50, 175, 235, 140) :
        IM_COL32(240, 70, 70, 140);

    const ImU32 outline = valid ?
        IM_COL32(60, 190, 255, 255) :
        IM_COL32(255, 85, 85, 255);

    for (const auto& cell : source.cells()) {
        const int cell_x = x + cell.offset.x;
        const int cell_y = y + cell.offset.y;
        math::vec2f world_position;
        if (automaton.topology().is_moore()) {
            world_position = {
                (static_cast<float>(cell_x) - automaton.width() * 0.5f) *
                    config.cell_size + config.offset.x,
                (static_cast<float>(cell_y) - automaton.height() * 0.5f) *
                    config.cell_size + config.offset.y
            };
        }
        else {
            world_position = active_viewport.layout().hex_to_world(
                cell_x - automaton.width() / 2,
                cell_y - automaton.height() / 2,
                config.cell_size, config.offset.x, config.offset.y);
        }
        const auto local = active_viewport.camera().world_to_screen(world_position);
        const ImVec2 center{viewport_pos.x + local.x, viewport_pos.y + local.y};
        const ImU32 cell_fill = valid ?
            (cell.state == 2 ? IM_COL32(245, 180, 65, 160) : fill) : fill;
        if (automaton.topology().is_moore()) {
            draw->AddRectFilled(
                {center.x - side * 0.5f, center.y - side * 0.5f},
                {center.x + side * 0.5f, center.y + side * 0.5f}, cell_fill);
            draw->AddRect(
                {center.x - side * 0.5f, center.y - side * 0.5f},
                {center.x + side * 0.5f, center.y + side * 0.5f}, outline);
        }
        else
            draw_pattern_hex(draw, center, side, cell_fill, outline);
    }
    draw->PopClipRect();
}

} // namespace cas::gui::panels
