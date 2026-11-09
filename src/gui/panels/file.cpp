#include <gui/panels/file.h>

#include <imgui.h>

namespace cas::gui::panels {

file::file(files::dialog_service& dialog_service)
    : _dialog_service(dialog_service) {
}

void file::render() {
    ImGui::Text("File");
    ImGui::Separator();

    if (ImGui::CollapsingHeader(
        "Save",
        ImGuiTreeNodeFlags_DefaultOpen
    )) {
        if (ImGui::Button("Save Automaton", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::save_automaton
            );
        }

        if (ImGui::Button("Save Viewport", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::save_viewport
            );
        }

        if (ImGui::Button("Save Workspace", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.request(
                files::file_operation::save_workspace
            );
        }

        if (ImGui::Button("Save Workspace As", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::save_workspace_as
            );
        }
    }

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::CollapsingHeader(
        "Load",
        ImGuiTreeNodeFlags_DefaultOpen
    )) {
        if (ImGui::Button("Load Automaton", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::load_automaton
            );
        }

        if (ImGui::Button("Load Viewport", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::load_viewport
            );
        }

        if (ImGui::Button("Load Workspace", ImVec2(-1.0f, 0.0f))) {
            _dialog_service.open_dialog(
                files::file_operation::load_workspace
            );
        }
    }
}

} // namespace cas::gui::panels