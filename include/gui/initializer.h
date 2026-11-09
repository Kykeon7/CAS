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

}

namespace cas::gui {

namespace panels {
    class collection;
}

class layout;
class settings_window;
class viewport_view;

class initializer {
public:
    void initialize(
        layout& layout,
        panels::collection& panels,
        viewport_view& viewport_view,
        viewports::manager& viewports,
        inputs::input& input,
        files::dialog_service& file_dialog_service,
        settings_window& settings_window,
        settings& settings,
        rendering::context& rendering_context
    ) const;
};

} // namespace cas::gui