#include <gui/panels/viewport.h>

#include <imgui.h>

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
    }

    profiler.end(system::profile_id::render);
}

} // namespace cas::gui::panels