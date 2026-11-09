#pragma once

#include <gui/panels/panel.h>
#include <gui/viewport_view.h>

namespace cas {
    struct stats_panel_settings;
}

namespace cas::rendering {
    class context;
}

namespace cas::gui::panels {

    class stats final : public panel {
    public:
        stats(
            viewport_view& view,
            const stats_panel_settings& settings,
            rendering::context& rendering_context
        );

        void render() override;

    private:
        viewport_view& _view;
        const stats_panel_settings& _settings;
        rendering::context& _rendering_context;
    };

} // namespace cas::gui::panels