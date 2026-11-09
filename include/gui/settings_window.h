#pragma once

#include <array>
#include <functional>
#include <optional>
#include <filesystem>

#include <application/settings.h>
#include <gui/settings_tab.h>
#include <gui/notifications.h>
#include <files/dialog_service.h>

namespace cas::gui {

class viewport_view;

class settings_window {
public:
    explicit settings_window(
        settings& settings,
        notifications& notifications,
        files::dialog_service& dialog_service
    );

    void set_viewport_view(viewport_view& view) noexcept;
    void set_save_directory(std::filesystem::path path);

    void open(settings_tab tab);
    void render();

private:
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
    void apply_randomization();

    void default_interface_draft();
    void reset_interface_draft();
    void apply_interface_draft();

    void render_appearance_preview();

    settings& _settings;
    notifications& _notifications;
    std::optional<std::reference_wrapper<viewport_view>> _view;

    bool _is_open = false;
    bool _select_requested_tab = false;
    settings_tab _active_tab = settings_tab::general;

    std::array<char, 256> _workspace_filename_buffer{};
    std::array<char, 512> _save_directory_buffer{};

    std::array<int, 2> _two_states_draft{ 50, 50 };
    std::array<int, 3> _three_states_draft{ 34, 33, 33 };
    std::uint32_t _random_seed_draft = 5489;

    bool _randomization_invalid = false;

    interface_settings _interface_draft;

    files::dialog_service& _dialog_service;
};

} // namespace cas