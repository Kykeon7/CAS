#include <gui/main_menu.h>

#include <imgui.h>

#include <gui/help_content.h>

namespace cas::gui {

main_menu::main_menu(
    files::dialog_service& dialog_service,
    panels::collection& panels,
    settings& settings
) :
    _dialog_service(dialog_service),
    _panels(panels),
    _settings(settings)
{
}

void main_menu::render(viewport_view& viewport_view) {
    if (!ImGui::BeginMainMenuBar())
        return;

    render_file_menu();

    if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginMenu("Panels")) {
            for (auto& entry : _panels.visibility_states()) {
                ImGui::MenuItem(
                    entry.name.c_str(),
                    nullptr,
                    &_panels.visibility(entry.name)
                );
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Reset Layout"))
            _layout_reset_requested = true;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Simulation")) {
        if (ImGui::MenuItem("Clean"))
            viewport_view.simulation().clean();

        if (ImGui::MenuItem("Randomize")) {
            viewport_view.simulation().randomize(
                _settings.randomization.seed
            );
        }

        ImGui::Separator();

        auto& simulation = viewport_view.simulation();

        if (ImGui::MenuItem(
            "Step",
            nullptr,
            false,
            !simulation.is_running()
        )) {
            simulation.step();
        }

        if (ImGui::MenuItem(
            simulation.is_running() ? "Pause" : "Run"
        )) {
            simulation.toggle();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Settings")) {
        if (ImGui::MenuItem("General"))
            _settings_request = settings_tab::general;

        if (ImGui::MenuItem("Simulation"))
            _settings_request = settings_tab::simulation;

        if (ImGui::MenuItem("Appearance"))
            _settings_request = settings_tab::appearance;

        if (ImGui::MenuItem("View"))
            _settings_request = settings_tab::view;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
        _help_window_open = true;
        ImGui::EndMenu();
    }

    if (_help_window_open) {
        if (ImGui::Begin(
            "Help##HelpPopup",
            &_help_window_open
        )) {
            render_help_content();
        }

        ImGui::End();
    }

    ImGui::EndMainMenuBar();
}

void main_menu::render_file_menu() {
    if (!ImGui::BeginMenu("File"))
        return;

    if (ImGui::BeginMenu("Save")) {
        if (ImGui::MenuItem("Automaton")) {
            _dialog_service.open_dialog(
                files::file_operation::save_automaton
            );
        }

        if (ImGui::MenuItem("Viewport")) {
            _dialog_service.open_dialog(
                files::file_operation::save_viewport
            );
        }

        if (ImGui::MenuItem("Workspace")) {
            _dialog_service.request(
                files::file_operation::save_workspace
            );
        }

        if (ImGui::MenuItem("Workspace As")) {
            _dialog_service.open_dialog(
                files::file_operation::save_workspace_as
            );
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Load")) {
        if (ImGui::MenuItem("Automaton")) {
            _dialog_service.open_dialog(
                files::file_operation::load_automaton
            );
        }

        if (ImGui::MenuItem("Viewport")) {
            _dialog_service.open_dialog(
                files::file_operation::load_viewport
            );
        }

        if (ImGui::MenuItem("Workspace")) {
            _dialog_service.open_dialog(
                files::file_operation::load_workspace
            );
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenu();
}

std::optional<settings_tab>
main_menu::consume_settings_request() noexcept {
    auto request = _settings_request;
    _settings_request.reset();
    return request;
}

bool main_menu::consume_layout_reset_request() noexcept {
    const bool requested = _layout_reset_requested;
    _layout_reset_requested = false;
    return requested;
}

} // namespace cas::gui