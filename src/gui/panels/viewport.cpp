#include <gui/panels/viewport.h>

#include <gui/panels/viewport_tabs.h>
#include <gui/panels/viewport_controls.h>
#include <system/profiler.h>
#include <imgui.h>

namespace cas::gui::panels {

viewport::viewport(
    cas::viewports::manager& viewports,
    const inputs::input& input
)
    : _viewports(viewports),
      _input(input) {
}

void viewport::render() {
    render_viewport_tabs(_viewports);

    auto& viewport = _viewports.active_viewport();

    // Controles superiores.
    render_zoom_controls(viewport);

    // Área de renderizado.
    const ImVec2 available = ImGui::GetContentRegionAvail();

    if (available.x <= 0.0f || available.y <= 0.0f)
        return;

    const ImVec2 viewport_pos = ImGui::GetCursorScreenPos();

    viewport.set_size({
        static_cast<int>(available.x),
        static_cast<int>(available.y)
        });

    const bool is_hovered = ImGui::IsWindowHovered();

    viewport.handle_input(
        _input,
        is_hovered,
        {
            viewport_pos.x,
            viewport_pos.y
        }
    );

    auto& profiler = system::profiler().instance();

    profiler.begin(system::profile_id::render);
    if (auto texture = viewport.render()) {
        ImGui::Image(
            static_cast<ImTextureID>(
                texture->get().imgui_id()
                ),
            available
        );
    }
    profiler.end(system::profile_id::render);
}

} // namespace cas::gui::panels