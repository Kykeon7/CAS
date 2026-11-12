#pragma once

namespace cas {

struct settings;

namespace files {
class dialog_service;
}

namespace inputs {
class input;
}

namespace rendering {
class context;
}

namespace viewports {
class manager;
}

} // namespace cas

namespace cas::gui {

namespace panels {
class collection;
}

class layout;
class settings_window;
class viewport_view;

/**
 * @brief Initializes the GUI panels and their layout containers.
 */
class initializer {
public:
    /**
     * @brief Creates and registers the GUI panels and containers.
     *
     * @param layout GUI layout manager.
     * @param panels Panel collection.
     * @param viewport_view Active viewport view.
     * @param viewports Viewport manager.
     * @param input Input system.
     * @param dialog_service File dialog service.
     * @param settings_window Application settings window.
     * @param settings Application settings.
     * @param rendering_context Rendering context.
     */
    void initialize(
        layout& layout,
        panels::collection& panels,
        viewport_view& viewport_view,
        viewports::manager& viewports,
        inputs::input& input,
        files::dialog_service& dialog_service,
        settings_window& settings_window,
        settings& settings,
        rendering::context& rendering_context
    ) const;
};

} // namespace cas::gui