#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>

#include <application/settings.h>
#include <files/dialog_service.h>
#include <gui/notifications.h>
#include <gui/settings_tab.h>

namespace cas::gui {

class viewport_view;

/**
 * @brief Displays and manages the application settings window.
 *
 * Maintains a temporary copy of application settings. Changes are applied
 * globally only after explicit confirmation.
 */
class settings_window {
public:
    /**
     * @brief Constructs the settings window.
     *
     * @param settings Application settings to manage.
     * @param notifications Notification service used to report changes.
     * @param dialog_service File dialog service used by the window.
     */
    explicit settings_window(
        settings& settings,
        notifications& notifications,
        files::dialog_service& dialog_service
    );

    settings_window(const settings_window&) = delete;
    settings_window& operator=(const settings_window&) = delete;
    settings_window(settings_window&&) = delete;
    settings_window& operator=(settings_window&&) = delete;

    ~settings_window() = default;

    /**
     * @brief Associates the viewport view used by simulation settings.
     *
     * @param view Viewport view to associate.
     */
    void set_viewport_view(viewport_view& view) noexcept;

    /**
     * @brief Updates the default save directory in the settings draft.
     *
     * @param path New default save directory.
     */
    void set_save_directory(std::filesystem::path path);

    /**
     * @brief Sets the backend currently active for this application session.
     *
     * @param backend Active rendering backend.
     * @param gpu_fallback Whether GPU initialization failed and CPU is in use.
     */
    void set_active_backend(
        rendering::backend backend,
        bool gpu_fallback = false
    ) noexcept;

    /**
     * @brief Opens the settings window and selects a tab.
     *
     * @param tab Tab to select.
     */
    void open(settings_tab tab);

    /**
     * @brief Renders the settings window.
     */
    void render();

private:
    bool has_pending_changes() const;
    bool validate_draft() const;

    void reset_draft();
    void apply_draft();

    void request_close();
    void request_apply();

    void render_actions();

    void render_general();
    void render_simulation();
    void render_appearance();
    void render_view();

    void default_randomization_draft();
    void reset_randomization_draft();
    void render_randomization();

    int render_randomization_distribution(
        const char* label,
        const char* id,
        int* values,
        int count
    );

    void default_interface_draft();
    void reset_interface_draft();

    void render_appearance_preview();

private:
    settings& _settings;
    settings _draft;

    notifications& _notifications;
    files::dialog_service& _dialog_service;

    rendering::backend _active_backend = rendering::backend::gpu;
    bool _gpu_fallback = false;

    std::optional<std::reference_wrapper<viewport_view>> _view;
    bool _toroidal_boundary_draft = false;

    bool _is_open = false;
    bool _select_requested_tab = false;
    bool _close_confirmation_pending = false;
    bool _apply_confirmation_pending = false;

    settings_tab _active_tab = settings_tab::general;

    std::array<char, 256> _workspace_filename_buffer{};
    std::array<char, 512> _save_directory_buffer{};

    std::array<int, 2> _two_states_draft{ 50, 50 };
    std::array<int, 3> _three_states_draft{ 34, 33, 33 };
    std::uint32_t _random_seed_draft = 5489;

    bool _randomization_invalid = false;
};

} // namespace cas::gui