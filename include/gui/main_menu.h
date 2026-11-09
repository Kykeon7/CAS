#pragma once

#include <gui/settings_tab.h>
#include <files/dialog_service.h>
#include <gui/panels/collection.h>
#include <optional>
#include <gui/viewport_view.h>
#include <application/settings.h>

namespace cas::gui {

class main_menu {
public:
    explicit main_menu(
        files::dialog_service& file_dialog_service,
        gui::panels::collection& panels,
        settings& settings
    );

    void render(viewport_view& viewport_view);

    std::optional<settings_tab> consume_settings_request() noexcept;

    bool consume_layout_reset_request() noexcept;

private:
    void render_file_menu();

    files::dialog_service& _dialog_service;
    gui::panels::collection& _panels;
    settings& _settings;

    std::optional<settings_tab> _settings_request;
    bool _layout_reset_requested = false;
    bool _help_window_open = false;
};

} // namespace cas