#include <application/application.h>

#include <SDL3/SDL.h>

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <chrono>
#include <utility>

#include <automata/automaton.h>
#include <automata/rules/rule_22_majority_memory.h>
#include <automata/topologies.h>
#include <automata/topology.h>
#include <files/serialization/automaton.h>
#include <files/serialization/settings.h>
#include <files/serialization/viewport.h>
#include <files/serialization/workspace.h>
#include <gui/manager.h>
#include <gui/panels/appearance.h>
#include <gui/panels/container.h>
#include <gui/panels/file.h>
#include <gui/panels/graphs.h>
#include <gui/panels/help.h>
#include <gui/panels/simulation.h>
#include <gui/panels/stats.h>
#include <gui/panels/viewport.h>
#include <gui/viewport_view.h>
#include <inputs/input.h>
#include <platform/gpu_renderer.h>
#include <rendering/automaton/gpu_renderer.h>
#include <rendering/automaton/hex_renderer.h>
#include <system/info.h>
#include <system/logger.h>
#include <system/profiler.h>
#include <system/time.h>
#include <viewports/camera.h>

namespace {

std::filesystem::path get_application_directory() {
    const char* base_path = SDL_GetBasePath();

    if (base_path == nullptr) {
        throw std::runtime_error(
            "Failed to retrieve application directory: "
            + std::string(SDL_GetError())
        );
    }

    return std::filesystem::path(base_path);
}

} // namespace

namespace cas {

application::application(rendering::backend backend)
    : _workspace(_settings.interface.scale),
      _window("CAS - Cellular Automaton Simulator"),
      _file_dialog_service(_window),
      _file_coordinator(
          _file_dialog_service,
          _file_operation_handler,
          _workspace
      ),
      _rendering_context(_window, backend),
      _settings_window(
          _settings,
          _notifications,
          _file_dialog_service
      ),
      _main_menu(_file_dialog_service, _panels, _settings)
      
{
    auto& logger = system::logger::instance();

    logger.info("Application", "Initializing CAS.");

    system::print_system_info();

    // Resolve portable application directories.
    const auto application_directory =
        get_application_directory();

    const auto config_directory =
        application_directory / "config";

    std::filesystem::create_directories(config_directory);

    _config_path = config_directory / "config.json";
    _imgui_ini_path = config_directory / "imgui.ini";

    // Load global application settings.
    if (std::filesystem::exists(_config_path)) {
        try {
            _settings =
                files::serialization::load_application_settings_file(
                    _config_path
                );

            logger.info(
                "Persistence",
                "Application settings loaded."
            );
        } catch (const std::exception& exception) {
            logger.error(
                "Persistence",
                "Failed to load application settings: "
                + std::string(exception.what())
            );

            _settings = {};
        }
    }

    // Configure the default workspace directory.
    if (_settings.default_save_directory.empty()) {
        _settings.default_save_directory =
            application_directory / "workspaces";
    }

    std::filesystem::create_directories(
        _settings.default_save_directory
    );

    // Schedule initial workspace loading.
    if (_settings.load_workspace_on_startup
        && !_settings.last_workspace_path.empty()
        && std::filesystem::exists(
            _settings.last_workspace_path
        )) {

        _workspace.set_location(_settings.last_workspace_path);

        _workspace_loading = true;
        _initial_workspace_load_pending = true;

        logger.info(
            "Persistence",
            "Workspace loading scheduled: "
            + _workspace.path().string()
        );
    }
    else {
        logger.info(
            "Persistence",
            "Starting with a default workspace."
        );

        _workspace.clear_location();

        const auto default_workspace_path =
            _settings.default_save_directory
            / _settings.workspace_filename;

        // Associate the new workspace only if its destination
        // does not already contain another workspace.
        if (!std::filesystem::exists(default_workspace_path)) {
            _workspace.set_location(default_workspace_path);

            _settings.last_workspace_path = _workspace.path();

            logger.info(
                "Persistence",
                "Default workspace location assigned: "
                + _workspace.path().string()
            );
        }
        else {
            logger.warn(
                "Persistence",
                "Default workspace already exists. "
                "Save As is required to avoid overwriting it."
            );
        }
    }

    update_window_title();

    initialize_backend();

    _gui->apply_interface_settings(_settings.interface);

    _layout = std::make_unique<gui::layout>(*_gui);

    _layout->initialize(
        _imgui_ini_path,
        _settings.save_imgui_layout
    );

    auto& viewports = _workspace.viewports();
    viewports.set_renderer_factory(
        [this](cas::viewports::viewport& viewport) {
            return _rendering_context.create_automaton_renderer(
                viewport
            );
        }
    );

    _viewport_view = std::make_unique<gui::viewport_view>(
        viewports.active_viewport()
    );

    _settings_window.set_viewport_view(*_viewport_view);

    _gui_initializer.initialize(
        *_layout,
        _panels,
        *_viewport_view,
        viewports,
        _input,
        _file_dialog_service,
        _settings_window,
        _settings,
        _rendering_context
    );

    _active_viewport = viewports.active_id();
    _prev_active_viewport = _active_viewport;

    _is_running = true;

    logger.info("Application", "CAS initialization completed.");
}

application::~application() {
    auto& logger = system::logger::instance();

    _file_operation_handler.wait_until_idle();
    _file_operation_handler.consume_results();

    if (_settings.save_workspace_on_exit
        && _workspace.has_location()) {
        try {
            files::serialization::save_workspace_file(
                _workspace.path(),
                _workspace.viewports(),
                _workspace.ui_scale()
            );

            _settings.last_workspace_path = _workspace.path();

            logger.info(
                "Persistence",
                "Workspace saved on exit: "
                + _workspace.path().string()
            );
        }
        catch (const std::exception& exception) {
            logger.error(
                "Persistence",
                "Failed to save workspace '"
                + _workspace.path().string()
                + "': "
                + exception.what()
            );
        }
    }

    if (_layout) {
        _layout->save();
    }

    try {
        files::serialization::save_application_settings_file(
            _config_path,
            _settings
        );

        logger.info(
            "Persistence",
            "Application settings saved."
        );
    }
    catch (const std::exception& exception) {
        logger.error(
            "Persistence",
            "Failed to save application settings: "
            + std::string(exception.what())
        );
    }
}

void application::run() {
    auto& time = time::instance();
    auto& profiler = system::profiler::instance();

    while (_is_running) {
        profiler.begin(system::profile_id::run);

        time.tick();
        _input.update();

        process_events();

        update();

        render();

        profiler.end(system::profile_id::run);
    }
}

void application::process_events() {
    SDL_Event event{};

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            _is_running = false;

        if (event.type == SDL_EVENT_MOUSE_WHEEL)
            _input.add_mouse_wheel(event.wheel.y);

        if (_gui)
            _gui->process_event(event);
    }
}

void application::update() {
    auto& viewports = _workspace.viewports();

    process_file_requests();

    start_initial_workspace_load();

    if (auto change = viewports.consume_active_change()) {
        auto& viewport = viewports.get(change->current);

        if (auto* renderer = _rendering_context.cpu_renderer()) {
            renderer->set_camera(viewport.camera());
        }

        _viewport_view->set_active_viewport(viewport);

        system::logger::instance().info(
            "Viewport",
            "Active viewport changed."
        );
    }

    for (auto& viewport : viewports.viewports()) {
        if (viewport)
            viewport->update_sim();
    }
}

void application::render() {
    if (auto* renderer = _rendering_context.cpu_renderer()) {
        renderer->clear(
            graphics::black
        );

        {
            auto frame = _gui->begin_frame(*renderer);

            _main_menu.render(*_viewport_view);

            if (_main_menu.consume_layout_reset_request()) {
                _panels.reset_visibility();
                _layout->reset_layout();
            }

            if (auto request = _main_menu.consume_settings_request()) {
                _settings_window.open(*request);
            }

            _layout->render(_panels);
            _settings_window.render();

            _layout->set_persistence_enabled(
                _settings.save_imgui_layout
            );

            _notifications.render();
        }

        renderer->present();
        return;
    }

    auto* renderer = _rendering_context.gpu_renderer();

    try {
        renderer->begin_frame();
        _gui->begin_frame();

        _main_menu.render(*_viewport_view);

        if (_main_menu.consume_layout_reset_request()) {
            _panels.reset_visibility();
            _layout->reset_layout();
        }

        if (auto request = _main_menu.consume_settings_request()) {
            _settings_window.open(*request);
        }

        _layout->render(_panels);
        _notifications.render();
        _settings_window.render();

        _layout->set_persistence_enabled(
            _settings.save_imgui_layout
        );

        _gui->prepare_draw_data(
            renderer->frame().command_buffer()
        );

        if (renderer->frame().has_swapchain_texture()) {
            auto* render_pass = renderer->frame().begin_render_pass();

            if (render_pass) {
                _gui->render_draw_data(
                    renderer->frame().command_buffer(),
                    render_pass
                );
            }
        }

        renderer->end_frame();
    }
    catch (...) {
        _gui->cancel_frame();
        renderer->cancel_frame();

        throw;
    }
}


void application::initialize_backend() {
    auto& logger = system::logger::instance();
    auto& viewport = _workspace.viewports().active_viewport();

    logger.info(
        "Renderer",
        _rendering_context.is_cpu_backend()
        ? "Initializing CPU rendering backend."
        : "Initializing GPU rendering backend."
    );

    _rendering_context.initialize(viewport.camera());

    if (auto* renderer = _rendering_context.cpu_renderer()) {
        _gui = std::make_unique<gui::manager>(
            _window,
            *renderer
        );

        logger.info("Renderer", "CPU backend initialized.");
        return;
    }

    auto* device = _rendering_context.gpu_device();

    _gui = std::make_unique<gui::manager>(*device);

    const char* device_name = device->name();

    logger.info(
        "Renderer",
        "GPU backend initialized: " +
        std::string(
            device_name != nullptr
            ? device_name
            : "Unknown"
        )
    );
}

void application::rebuild_panels() {
    _layout->clear();
    _panels.clear();

    _gui_initializer.initialize(
        *_layout,
        _panels,
        *_viewport_view,
        _workspace.viewports(),
        _input,
        _file_dialog_service,
        _settings_window,
        _settings,
        _rendering_context
    );
}


void application::process_file_requests() {
    for (auto& result :
        _file_operation_handler.consume_results()) {
        process_file_result(std::move(result));
    }

    const auto request = _file_dialog_service.consume();

    if (request.has_value())
        process_file_request(*request);
}


void application::process_file_request(
    const files::file_request& request
) {
    using files::file_operation;
    using files::request_status;

    if (request.operation == file_operation::select_save_directory) {
        std::error_code error;

        if (!std::filesystem::is_directory(request.path, error) || error) {
            _notifications.error(
                "Settings",
                "The selected path is not a valid directory."
            );

            return;
        }

        _settings_window.set_save_directory(request.path);

        _notifications.info(
            "Settings",
            "Default save directory updated."
        );

        return;
    }

    // El resto de tu implementación actual continúa aquí.

    const auto make_progress_callback =
        [this](file_operation operation) {
        gui::notification_id id;

        switch (operation) {
        case file_operation::load_automaton:
            _automaton_load_notification =
                _notifications.begin_blocking(
                    "Persistence",
                    "Loading automaton..."
                );
            id = *_automaton_load_notification;
            break;

        case file_operation::load_viewport:
            _viewport_load_notification =
                _notifications.begin_blocking(
                    "Persistence",
                    "Loading viewport..."
                );
            id = *_viewport_load_notification;
            break;

        case file_operation::load_workspace:
            _workspace_load_notification =
                _notifications.begin_blocking(
                    "Workspace",
                    "Loading workspace..."
                );
            id = *_workspace_load_notification;
            break;

        default:
            return files::serialization::progress_callback{};
        }

        return files::serialization::progress_callback{
            [this, id](
                std::uint64_t completed,
                std::uint64_t total,
                std::string_view stage
            ) {
                _notifications.update_progress(
                    id,
                    completed,
                    total,
                    stage
                );
            }
        };
    };

    const auto status = _file_coordinator.submit_request(
        request,
        make_progress_callback
    );

    if (status == request_status::rejected_no_active_viewport) {
        switch (request.operation) {
        case file_operation::save_automaton:
            _notifications.error(
                "Persistence",
                "Cannot save automaton: no active viewport."
            );
            break;

        case file_operation::load_automaton:
            _notifications.error(
                "Persistence",
                "Cannot load automaton: no active viewport."
            );
            break;

        case file_operation::save_viewport:
            _notifications.error(
                "Persistence",
                "Cannot save viewport: no active viewport."
            );
            break;

        case file_operation::load_viewport:
            _notifications.error(
                "Persistence",
                "Cannot load viewport: no active viewport."
            );
            break;

        default:
            break;
        }

        return;
    }

    if (status == request_status::redirected_to_save_as) {
        return;
    }

    switch (request.operation) {
    case file_operation::save_automaton:
        _notifications.info(
            "Persistence",
            "Saving automaton..."
        );
        break;

    case file_operation::save_viewport:
        _notifications.info(
            "Persistence",
            "Saving viewport..."
        );
        break;

    case file_operation::save_workspace:
        _notifications.info(
            "Persistence",
            "Saving workspace..."
        );
        break;

    case file_operation::save_workspace_as:
        _notifications.info(
            "Persistence",
            "Saving workspace to a new location..."
        );
        break;

    default:
        break;
    }
}


void application::process_file_result(
    files::file_operation_result result
) {
    using files::file_operation;
    using files::file_operation_action;


    if (result.action == file_operation_action::failed) {
        switch (result.operation) {
        case file_operation::load_automaton:
            if (_automaton_load_notification) {
                _notifications.dismiss(
                    *_automaton_load_notification
                );
                _automaton_load_notification.reset();
            }
            break;

        case file_operation::load_viewport:
            if (_viewport_load_notification) {
                _notifications.dismiss(
                    *_viewport_load_notification
                );
                _viewport_load_notification.reset();
            }
            break;

        case file_operation::load_workspace:
            if (_workspace_load_notification) {
                _notifications.dismiss(
                    *_workspace_load_notification
                );
                _workspace_load_notification.reset();
            }

            _workspace_loading = false;
            break;

        default:
            break;
        }

        _notifications.error(
            "Persistence",
            "File operation failed: " + result.path.string() +
            "\n" + result.error_message
        );

        return;
    }

    const auto status = _file_coordinator.process_result(result);

    switch (status) {
    case files::result_status::workspace_saved:
        _settings.last_workspace_path = result.path;

        update_window_title();

        _notifications.info(
            "Persistence",
            "Workspace saved successfully."
        );

        return;

    case files::result_status::save_as_requested:
        return;

    case files::result_status::not_handled:
        break;
    }

    switch (result.operation) {
    case files::file_operation::load_automaton:
        if (_automaton_load_notification) {
            _notifications.dismiss(*_automaton_load_notification);
            _automaton_load_notification.reset();
        }
        break;

    case files::file_operation::load_viewport:
        if (_viewport_load_notification) {
            _notifications.dismiss(*_viewport_load_notification);
            _viewport_load_notification.reset();
        }
        break;

    case files::file_operation::load_workspace:
        // Keep the blocking notification alive until
        // workspace installation has finished.
        break;

    default:
        break;
    }

    switch (result.action) {
    case file_operation_action::completed:
        switch (result.operation) {
        case file_operation::save_automaton:
            _notifications.info(
                "Persistence",
                "Automaton saved successfully."
            );
            break;

        case file_operation::save_viewport:
            _notifications.info(
                "Persistence",
                "Viewport saved successfully."
            );
            break;

        default:
            break;
        }
        break;

    case file_operation_action::loaded_automaton: {
        if (!result.loaded.has_value()) {
            _notifications.error(
                "Persistence",
                "Automaton loading returned no data: " +
                result.path.string()
            );
            return;
        }

        auto* loaded =
            std::get_if<std::unique_ptr<automata::automaton>>(
                &*result.loaded
            );

        if (loaded == nullptr || !*loaded) {
            _notifications.error(
                "Persistence",
                "Invalid data returned while loading automaton: " +
                result.path.string()
            );
            return;
        }

        const auto target_viewport = result.target_viewport;
        auto& viewports = _workspace.viewports().viewports();

        if (target_viewport >= viewports.size()
            || !viewports[target_viewport].has_value()) {
            _notifications.error(
                "Persistence",
                "Cannot load automaton: target viewport no longer exists."
            );
            return;
        }

        auto& target = *viewports[target_viewport];

        auto& simulation = target.simulation();

        // Detener la simulación antes de reemplazar el autómata.
        if (simulation.is_running()) {
            simulation.toggle();
        }

        // Instalar el autómata cargado.
        simulation.automaton() = std::move(**loaded);

        // Limpiar los historiales para evitar mezclar datos
        // del autómata anterior con los del nuevo.
        target.population_history().clear();
        target.density_history().clear();
        target.activity_history().clear();

        _notifications.info(
            "Persistence",
            "Automaton loaded successfully."
        );

        break;
    }

    case file_operation_action::loaded_viewport: {
        if (!result.loaded.has_value()) {
            _notifications.error(
                "Persistence",
                "Viewport loading returned no data: " +
                result.path.string()
            );
            return;
        }

        auto* loaded =
            std::get_if<std::unique_ptr<cas::viewports::viewport>>(
                &*result.loaded
            );

        if (loaded == nullptr || !*loaded) {
            _notifications.error(
                "Persistence",
                "Invalid data returned while loading viewport: " +
                result.path.string()
            );
            return;
        }

        load_viewport(std::move(**loaded), result.path);

        _notifications.info(
            "Persistence",
            "Viewport loaded successfully."
        );
        break;
    }


    case file_operation_action::loaded_workspace: {
        const auto dismiss_notification = [this]() {
            if (_workspace_load_notification) {
                _notifications.dismiss(
                    *_workspace_load_notification
                );

                _workspace_load_notification.reset();
            }
            };

        if (!result.loaded.has_value()) {
            dismiss_notification();
            _workspace_loading = false;

            _notifications.error(
                "Persistence",
                "Workspace loading returned no data: "
                + result.path.string()
            );

            return;
        }

        auto* loaded =
            std::get_if<
            std::unique_ptr<files::serialization::workspace_data>
            >(&*result.loaded);

        if (loaded == nullptr || !*loaded) {
            dismiss_notification();
            _workspace_loading = false;

            _notifications.error(
                "Persistence",
                "Invalid data returned while loading workspace: "
                + result.path.string()
            );

            return;
        }

        try {
            load_workspace(
                std::move(**loaded),
                result.path
            );
        }
        catch (const std::exception& exception) {
            dismiss_notification();
            _workspace_loading = false;

            _notifications.error(
                "Persistence",
                "Failed to install workspace: "
                + result.path.string()
                + "\n"
                + exception.what()
            );

            return;
        }

        dismiss_notification();
        _workspace_loading = false;

        _notifications.info(
            "Workspace",
            "Workspace loaded successfully."
        );

        break;
    }

    case file_operation_action::request_save_workspace_as:
        // Handled by files::coordinator.
        break;

    case file_operation_action::failed:
        // Los errores ya se gestionaron al inicio del método.
        break;
    }
}

void application::update_window_title() {
    constexpr std::string_view base_title =
        "CAS - Cellular Automaton Simulator - ";

    const auto default_path =
        (_settings.default_save_directory /
            _settings.workspace_filename).lexically_normal();

    const auto workspace_path =
        _workspace.has_location()
        ? _workspace.path().lexically_normal()
        : default_path;

    const bool is_default_workspace =
        workspace_path == default_path;

    const auto filename =
        workspace_path.filename().u8string();

    std::string title(base_title);

    title.append(
        reinterpret_cast<const char*>(filename.data()),
        filename.size()
    );

    if (is_default_workspace)
        title += '*';

    _window.set_title(title);
}

void application::load_viewport(
    viewports::viewport loaded_viewport,
    const std::filesystem::path& path
) {
    auto& viewports = _workspace.viewports();

    loaded_viewport.set_renderer(
        _rendering_context.create_automaton_renderer(
            loaded_viewport
        )
    );

    const auto active_id = viewports.active_id();

    _layout->clear();
    _panels.clear();

    viewports.get(active_id) = std::move(loaded_viewport);

    auto& active_viewport = viewports.active_viewport();

    _viewport_view->set_active_viewport(active_viewport);

    if (auto* renderer = _rendering_context.cpu_renderer()) {
        renderer->set_camera(active_viewport.camera());
    }

    _active_viewport = viewports.active_id();
    _prev_active_viewport = _active_viewport;

    viewports.consume_active_change();

    rebuild_panels();

    system::logger::instance().info(
        "Persistence",
        "Active viewport replaced: " + path.string()
    );
}


void application::load_workspace(
    files::serialization::workspace_data loaded_workspace,
    const std::filesystem::path& path
) {
    using clock = std::chrono::steady_clock;

    const auto log_duration = [](
        const char* stage,
        clock::time_point start
        ) {
            const auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    clock::now() - start
                ).count();

            system::logger::instance().info(
                "Workspace",
                std::string(stage) + ": "
                + std::to_string(elapsed) + " ms"
            );
        };

    auto start = clock::now();

    auto& viewports = _workspace.viewports();

    _layout->clear();
    _panels.clear();

    viewports = std::move(loaded_workspace.viewports);
    _workspace.set_ui_scale(loaded_workspace.ui_scale);

    log_duration("State replacement", start);

    start = clock::now();

    viewports.set_renderer_factory(
        [this](cas::viewports::viewport& viewport) {
            return _rendering_context.create_automaton_renderer(
                viewport
            );
        }
    );

    log_duration("Renderer creation", start);

    start = clock::now();

    _viewport_view->set_active_viewport(
        viewports.active_viewport()
    );

    _active_viewport = viewports.active_id();
    _prev_active_viewport = _active_viewport;

    viewports.consume_active_change();

    if (auto* renderer = _rendering_context.cpu_renderer()) {
        renderer->set_camera(
            viewports.active_viewport().camera()
        );
    }

    log_duration("Active viewport setup", start);

    start = clock::now();

    _workspace.set_location(path);
    _settings.last_workspace_path = path;

    rebuild_panels();

    log_duration("Panel rebuilding", start);

    update_window_title();

    system::logger::instance().info(
        "Persistence",
        "Workspace replaced: " + path.string()
    );


}

void application::start_initial_workspace_load() {
    if (!_initial_workspace_load_pending) {
        return;
    }

    _initial_workspace_load_pending = false;

    _workspace_load_notification = _notifications.begin_blocking(
        "Workspace",
        "Loading workspace..."
    );

    files::file_job job;
    job.operation = files::file_operation::load_workspace;
    job.path = _workspace.path();
    

    job.on_progress = [this, id = *_workspace_load_notification](
        std::uint64_t completed,
        std::uint64_t total,
        std::string_view stage
        ) {
            _notifications.update_progress(
                id,
                completed,
                total,
                stage
            );
        };

    _file_operation_handler.submit(std::move(job));
}

} // namespace cas