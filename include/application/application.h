#pragma once

#include <concepts>
#include <filesystem>
#include <memory>

#include <gui/layout.h>
#include <gui/notifications.h>
#include <gui/panels/panel.h>
#include <inputs/input.h>
#include <rendering/context.h>
#include <viewports/manager.h>
#include <gui/main_menu.h>
#include <platform/sdl_context.h>
#include <platform/window.h>
#include <gui/panels/collection.h>
#include <gui/settings_window.h>
#include <files/dialog_service.h>
#include <files/operation_handler.h>
#include <gui/initializer.h>
#include <application/settings.h>
#include <workspace/workspace.h>
#include <files/coordinator.h>

namespace cas {

class manager;
class viewport_view;

class application {
public:
    explicit application(
        rendering::backend backend =
            rendering::backend::gpu
    );

    ~application();

    void run();

private:
    void process_events();
    void update();
    void render();
    void initialize_backend();

    void process_file_requests();
    void process_file_request(const files::file_request& request);
    void process_file_result(files::file_operation_result result);

    void update_window_title();

    void rebuild_panels();

    void load_workspace(
        files::serialization::workspace_data loaded_workspace,
        const std::filesystem::path& path
    );

    void load_viewport(
        viewports::viewport loaded_viewport,
        const std::filesystem::path& path
    );

    template <typename T, typename... Args>
        requires std::derived_from<T, gui::panels::panel>
    T& create_panel(Args&&... args) {
        return _panels.create<T>(
            std::forward<Args>(args)...
        );
    }

    void start_initial_workspace_load();

private:
    settings _settings;
    workspace _workspace;

    platform::sdl_context _sdl{
        SDL_INIT_VIDEO | SDL_INIT_EVENTS
    };

    platform::window _window;
    files::dialog_service _file_dialog_service;
    files::operation_handler _file_operation_handler;
    files::coordinator _file_coordinator;

    rendering::context _rendering_context;

    std::unique_ptr<gui::manager> _gui;

    std::unique_ptr<gui::viewport_view> _viewport_view;

    viewports::manager::viewport_id _active_viewport = 0;
    viewports::manager::viewport_id _prev_active_viewport = 0;

    inputs::input _input;
    bool _is_running = false;

    gui::panels::collection _panels;
    std::unique_ptr<gui::layout> _layout;

    gui::initializer _gui_initializer;

    std::filesystem::path _config_path;
    std::filesystem::path _imgui_ini_path;

    bool _workspace_loading = false;
    bool _initial_workspace_load_pending = false;

    gui::notifications _notifications;
    std::optional<gui::notification_id> _workspace_load_notification;
    std::optional<gui::notification_id> _viewport_load_notification;
    std::optional<gui::notification_id> _automaton_load_notification;

    gui::main_menu _main_menu;
    gui::settings_window _settings_window;
};

} // namespace cas
