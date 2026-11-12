#pragma once

#include <optional>

#include <application/settings.h>
#include <files/dialog_service.h>
#include <gui/panels/collection.h>
#include <gui/settings_tab.h>
#include <gui/viewport_view.h>

namespace cas::gui {

/**
 * @brief Manages the application's main menu and its actions.
 */
class main_menu {
public:
    /**
     * @brief Constructs the main menu.
     *
     * @param dialog_service File dialog service.
     * @param panels Panel collection.
     * @param settings Application settings.
     */
    explicit main_menu(
        files::dialog_service& dialog_service,
        panels::collection& panels,
        settings& settings
    );

    main_menu(const main_menu&) = delete;
    main_menu& operator=(const main_menu&) = delete;

    main_menu(main_menu&&) = delete;
    main_menu& operator=(main_menu&&) = delete;

    ~main_menu() = default;

    /**
     * @brief Renders the main menu and processes user interactions.
     *
     * @param viewport_view Active viewport view.
     */
    void render(viewport_view& viewport_view);

    /**
     * @brief Retrieves and clears the pending settings request.
     *
     * @return Requested settings tab, or std::nullopt if none exists.
     */
    [[nodiscard]]
    std::optional<settings_tab> consume_settings_request() noexcept;

    /**
     * @brief Retrieves and clears the pending layout reset request.
     *
     * @return True if a layout reset was requested.
     */
    [[nodiscard]]
    bool consume_layout_reset_request() noexcept;

private:
    void render_file_menu();

    files::dialog_service& _dialog_service;
    panels::collection& _panels;
    settings& _settings;

    std::optional<settings_tab> _settings_request;
    bool _layout_reset_requested = false;
    bool _help_window_open = false;
};

} // namespace cas::gui