
#include <gui/layout.h>

#include <gui/manager.h>

#include <gui/panels/collection.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>
#include <utility>

namespace cas::gui {

layout::layout(manager& gui)
    : _gui(gui)
{}


void layout::build_default_layout() {
    const ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::DockBuilderRemoveNode(dockspace_id);

    ImGui::DockBuilderAddNode(
        dockspace_id,
        ImGuiDockNodeFlags_DockSpace
    );

    ImGui::DockBuilderSetNodePos(
        dockspace_id,
        viewport->WorkPos
    );

    ImGui::DockBuilderSetNodeSize(
        dockspace_id,
        viewport->WorkSize
    );

    ImGuiID dock_main = dockspace_id;

    const ImGuiID dock_left = ImGui::DockBuilderSplitNode(
        dock_main,
        ImGuiDir_Left,
        0.20f,
        nullptr,
        &dock_main
    );

    const ImGuiID dock_right = ImGui::DockBuilderSplitNode(
        dock_main,
        ImGuiDir_Right,
        0.25f,
        nullptr,
        &dock_main
    );

    const ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(
        dock_main,
        ImGuiDir_Down,
        0.25f,
        nullptr,
        &dock_main
    );

    ImGui::DockBuilderDockWindow("Simulation", dock_left);
    ImGui::DockBuilderDockWindow("Appearance", dock_left);
    ImGui::DockBuilderDockWindow("File", dock_left);

    ImGui::DockBuilderDockWindow("Help", dock_right);
    ImGui::DockBuilderDockWindow("Stats", dock_right);

    ImGui::DockBuilderDockWindow("Graphs", dock_bottom);
    ImGui::DockBuilderDockWindow("Viewport", dock_main);

    ImGui::DockBuilderGetNode(dock_bottom)->LocalFlags |=
        ImGuiDockNodeFlags_HiddenTabBar;

    ImGui::DockBuilderGetNode(dock_main)->LocalFlags |=
        ImGuiDockNodeFlags_HiddenTabBar;

    ImGui::DockBuilderFinish(dockspace_id);
}


gui::panels::container& layout::create_container(std::string name) {
    auto container = std::make_unique<gui::panels::container>(
        std::move(name)
    );

    auto& container_ref = *container;
    _containers.emplace_back(std::move(container));

    return container_ref;
}

bool layout::initialize(
    const std::filesystem::path& ini_path,
    bool persistence_enabled
) {
    _ini_path = ini_path;
    _persistence_enabled = persistence_enabled;

    ImGui::GetIO().IniFilename = nullptr;

    _has_saved_layout =
        _persistence_enabled &&
        std::filesystem::exists(_ini_path);

    _force_default_layout = !_has_saved_layout;

    if (!_has_saved_layout) {
        return false;
    }

    ImGui::LoadIniSettingsFromDisk(
        _ini_path.string().c_str()
    );

    return true;
}

bool layout::load() {
    if (!_persistence_enabled
        || !std::filesystem::exists(_ini_path)) {
        return false;
    }

    ImGui::LoadIniSettingsFromDisk(
        _ini_path.string().c_str()
    );

    return true;
}

bool layout::save() const {
    if (!_persistence_enabled) {
        return false;
    }

    ImGui::SaveIniSettingsToDisk(
        _ini_path.string().c_str()
    );

    return true;
}

void layout::reset_layout() {
    _force_default_layout = true;
}

void layout::clear() {
    _containers.clear();
    _force_default_layout = true;
}

void layout::render(gui::panels::collection& panels) {
    _gui.begin_dockspace();

    if (_force_default_layout) {
        build_default_layout();
        _force_default_layout = false;
    }

    _gui.submit_dockspace();

    for (auto& container : _containers) {
        container->render(
            panels.visibility(container->name())
        );
    }

    _gui.end_dockspace();
}

void layout::set_persistence_enabled(bool enabled) noexcept {
    _persistence_enabled = enabled;
}

} // namespace cas
