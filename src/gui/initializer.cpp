
#include <gui/initializer.h>
#include <gui/layout.h>
#include <gui/panels/collection.h>
#include <gui/settings_window.h>
#include <gui/panels/simulation.h>
#include <gui/panels/stats.h>
#include <gui/panels/appearance.h>
#include <gui/panels/graphs.h>
#include <gui/panels/help.h>
#include <gui/panels/viewport.h>
#include <gui/panels/file.h>
#include <inputs/input.h>
#include <viewports/manager.h>
#include <gui/viewport_view.h>
#include <files/dialog_service.h>
#include <application/settings.h>
#include <rendering/context.h>

namespace cas::gui {

void initializer::initialize(
    layout& layout,
    panels::collection& panels,
    viewport_view& viewport_view,
    viewports::manager& viewports,
    inputs::input& input,
    files::dialog_service& dialog_service,
    settings_window& settings_window,
    settings& settings,
    rendering::context& rendering_context
) const {
    // Create panels.
    panels.create<panels::simulation>(
        viewport_view,
        settings,
        [&settings_window](settings_tab tab) {
            settings_window.open(tab);
        }
    );

    panels.create<panels::stats>(
        viewport_view,
        settings.stats,
        rendering_context
    );

    panels.create<panels::appearance>(viewport_view);
    panels.create<panels::graphs>(viewport_view);
    panels.create<panels::help>();
    panels.create<panels::file>(dialog_service);
    panels.create<panels::viewport>(viewports, input);

    panels.register_panel("Simulation");
    panels.register_panel("Stats");
    panels.register_panel("Appearance");
    panels.register_panel("Graphs");
    panels.register_panel("Help");
    panels.register_panel("File");
    panels.register_panel("Viewport");

    // Create containers and associate their panels.
    layout.create_container("Simulation").add_panel(
        panels.get<panels::simulation>()
    );

    layout.create_container("Stats").add_panel(
        panels.get<panels::stats>()
    );

    layout.create_container("Appearance").add_panel(
        panels.get<panels::appearance>()
    );

    layout.create_container("Graphs").add_panel(
        panels.get<panels::graphs>()
    );

    layout.create_container("Help").add_panel(
        panels.get<panels::help>()
    );

    layout.create_container("File").add_panel(
        panels.get<panels::file>()
    );

    layout.create_container("Viewport").add_panel(
        panels.get<panels::viewport>()
    );
}

} // namespace cas
