
#pragma once

#include <SDL3/SDL.h>

#include <concepts>
#include <filesystem>
#include <memory>
#include <optional>
#include <utility>

#include <application/settings.h>
#include <files/coordinator.h>
#include <files/dialog_service.h>
#include <files/operation_handler.h>
#include <files/serialization/workspace.h>
#include <gui/initializer.h>
#include <gui/layout.h>
#include <gui/main_menu.h>
#include <gui/notifications.h>
#include <gui/panels/collection.h>
#include <gui/panels/panel.h>
#include <gui/settings_window.h>
#include <inputs/input.h>
#include <platform/sdl_context.h>
#include <platform/window.h>
#include <rendering/context.h>
#include <viewports/manager.h>
#include <workspace/workspace.h>

namespace cas {

class manager;
class viewport_view;

/**
 * @brief Coordinates the application lifecycle and its main subsystems.
 *
 * Manages SDL initialization, rendering backend setup, GUI construction,
 * event processing, simulation updates, rendering, and file operations.
 *
 * Owns the application settings, workspace, window, rendering context,
 * GUI components, and file operation services.
 */
class application {
public:
    /**
     * @brief Constructs and initializes the application.
     *
     * Initializes SDL, creates the application window, loads application
     * settings, configures the workspace, initializes the rendering backend,
     * and constructs the GUI.
     *
     * @param backend Rendering backend to initialize.
     */
    explicit application(
        rendering::backend backend =
            rendering::backend::gpu
    );

    /**
     * @brief Shuts down the application and saves persistent state.
     *
     * Waits for pending file operations to finish, saves the workspace
     * when configured to do so, and persists application settings.
     */
    ~application();

    application(const application&) = delete;
    application& operator=(const application&) = delete;
    application(application&&) = delete;
    application& operator=(application&&) = delete;

    /**
     * @brief Runs the main application loop.
     *
     * Processes input and events, updates the application state,
     * and renders frames until the application is requested to stop.
     */
    void run();

private:
    /**
     * @brief Processes pending SDL events.
     *
     * Handles application quit events and forwards events to the input
     * and GUI systems.
     */
    void process_events();

    /**
     * @brief Updates the application state.
     *
     * Processes file requests, starts pending workspace loading,
     * handles active viewport changes, and updates viewport simulations.
     */
    void update();

    /**
     * @brief Renders the current application frame.
     *
     * Uses the active rendering backend to render the GUI and its contents.
     */
    void render();

    /**
     * @brief Initializes the selected rendering backend and GUI manager.
     */
    void initialize_backend();

    /**
     * @brief Processes pending file requests and operation results.
     */
    void process_file_requests();

    /**
     * @brief Handles a file request submitted by the GUI.
     *
     * Validates directory-selection requests and submits supported
     * file operations through the file coordinator.
     *
     * @param request File operation request to process.
     */
    void process_file_request(const files::file_request& request);

    /**
     * @brief Handles the result of a completed file operation.
     *
     * Processes operation failures and installs successfully loaded
     * automata, viewports, or workspaces.
     *
     * @param result Result returned by the file operation handler.
     */
    void process_file_result(files::file_operation_result result);

    /**
     * @brief Updates the application window title.
     *
     * Reflects the current workspace filename and indicates when
     * the displayed location corresponds to the default workspace.
     */
    void update_window_title();

    /**
     * @brief Recreates the GUI panels using the current application state.
     */
    void rebuild_panels();

    /**
     * @brief Replaces the current workspace with the loaded workspace data.
     *
     * Installs the loaded viewports, restores the UI scale, recreates
     * viewport renderers, updates the active viewport, and rebuilds
     * the GUI panels.
     *
     * @param loaded_workspace Workspace data to install.
     * @param path Filesystem location from which the workspace was loaded.
     */
    void load_workspace(
        files::serialization::workspace_data loaded_workspace,
        const std::filesystem::path& path
    );

    /**
     * @brief Replaces the active viewport with a loaded viewport.
     *
     * Creates its rendering resources, updates the active viewport,
     * and rebuilds the GUI panels.
     *
     * @param loaded_viewport Viewport to install.
     * @param path Filesystem location from which the viewport was loaded.
     */
    void load_viewport(
        viewports::viewport loaded_viewport,
        const std::filesystem::path& path
    );

    /**
     * @brief Creates and registers a panel of the specified type.
     *
     * Forwards the supplied arguments to the panel collection.
     *
     * @tparam T Concrete panel type derived from gui::panels::panel.
     * @tparam Args Argument types forwarded to the panel constructor.
     * @param args Arguments forwarded to the panel constructor.
     * @return Reference to the newly created panel.
     */
    template <typename T, typename... Args>
        requires std::derived_from<T, gui::panels::panel>
    T& create_panel(Args&&... args) {
        return _panels.create<T>(
            std::forward<Args>(args)...
        );
    }

    /**
     * @brief Starts the deferred initial workspace loading operation.
     *
     * Submits the workspace loading job when an initial load was
     * scheduled during application initialization.
     */
    void start_initial_workspace_load();

private:
    settings _settings;

    platform::sdl_context _sdl{
        SDL_INIT_VIDEO | SDL_INIT_EVENTS
    };

    platform::window _window;
    files::dialog_service _file_dialog_service;
    files::operation_handler _file_operation_handler;
    files::coordinator _file_coordinator;

    rendering::context _rendering_context;

    std::unique_ptr<gui::manager> _gui;

    workspace _workspace;

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
