#include <gui/settings_window.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <numeric>

#include <gui/style.h>
#include <gui/viewport_view.h>

namespace cas::gui {
namespace {

class interface_preview_scope {
public:
    explicit interface_preview_scope(
        const interface_settings& settings
    ) :
        _previous_style(ImGui::GetStyle()),
        _previous_font_scale(ImGui::GetIO().FontGlobalScale)
    {
        apply_interface_style(settings);

        // Child windows must have an opaque background
        // to prevent the parent window from showing through.
        ImGui::GetStyle().Colors[ImGuiCol_ChildBg] =
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    }

    interface_preview_scope(const interface_preview_scope&) = delete;
    interface_preview_scope& operator=(
        const interface_preview_scope&
        ) = delete;

    interface_preview_scope(interface_preview_scope&&) = delete;
    interface_preview_scope& operator=(
        interface_preview_scope&&
        ) = delete;

    ~interface_preview_scope() {
        ImGui::GetStyle() = _previous_style;
        ImGui::GetIO().FontGlobalScale = _previous_font_scale;
    }

private:
    ImGuiStyle _previous_style;
    float _previous_font_scale;
};

} // namespace 

settings_window::settings_window(
    settings& settings,
    notifications& notifications,
    files::dialog_service& dialog_service
)
    : _settings(settings),
    _notifications(notifications),
    _dialog_service(dialog_service)
{

    const auto& filename = _settings.workspace_filename;

    const auto filename_length = std::min(
        filename.size(),
        _workspace_filename_buffer.size() - 1
    );

    std::memcpy(
        _workspace_filename_buffer.data(),
        filename.data(),
        filename_length
    );

    _workspace_filename_buffer[filename_length] = '\0';

    const auto directory = _settings.default_save_directory.string();

    const auto directory_length = std::min(
        directory.size(),
        _save_directory_buffer.size() - 1
    );

    std::memcpy(
        _save_directory_buffer.data(),
        directory.data(),
        directory_length
    );

    _save_directory_buffer[directory_length] = '\0';

    reset_randomization_draft();
}

void settings_window::set_save_directory(
    std::filesystem::path path
) {
    _settings.default_save_directory = path;

    const auto directory = path.string();

    const auto length = std::min(
        directory.size(),
        _save_directory_buffer.size() - 1
    );

    std::memcpy(
        _save_directory_buffer.data(),
        directory.data(),
        length
    );

    _save_directory_buffer[length] = '\0';
}

void settings_window::set_viewport_view(
    viewport_view& view
) noexcept {
    _view = std::ref(view);
}

void settings_window::open(settings_tab tab) {
    if (!_is_open) {
        if (tab == settings_tab::simulation)
            reset_randomization_draft();

        if (tab == settings_tab::appearance)
            reset_interface_draft();
    }

    _active_tab = tab;
    _is_open = true;
    _select_requested_tab = true;
}

void settings_window::default_randomization_draft() {
    _two_states_draft = { 50, 50 };
    _three_states_draft = { 34, 33, 33 };
    _random_seed_draft = 5489;

    _randomization_invalid = false;
}

void settings_window::reset_randomization_draft() {
    const auto& randomization = _settings.randomization;

    for (std::size_t i = 0; i < _two_states_draft.size(); ++i) {
        _two_states_draft[i] =
            static_cast<int>(randomization.two_states[i]);
    }

    for (std::size_t i = 0; i < _three_states_draft.size(); ++i) {
        _three_states_draft[i] =
            static_cast<int>(randomization.three_states[i]);
    }

    _random_seed_draft = _settings.randomization.seed;

    _randomization_invalid = false;
}

int settings_window::render_randomization_distribution(
    const char* label,
    const char* id,
    int* values,
    int count
) {
    ImGui::PushID(id);

    ImGui::Text("%s", label);

    int total = 0;

    for (int i = 0; i < count; ++i) {
        ImGui::PushID(i);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("State %d", i);

        ImGui::SameLine();

        ImGui::SetNextItemWidth(
            ImGui::GetContentRegionAvail().x - 100.0f
        );

        ImGui::SliderInt(
            "##Slider",
            &values[i],
            0,
            100,
            "%d%%"
        );

        ImGui::SameLine();

        ImGui::SetNextItemWidth(90.0f);

        ImGui::InputInt(
            "##Input",
            &values[i],
            1,
            5
        );

        values[i] = std::clamp(values[i], 0, 100);

        total += values[i];

        ImGui::PopID();
    }

    ImGui::Text(
        "Total: %d%%",
        total
    );

    ImGui::PopID();

    return total;
}

void settings_window::apply_randomization() {
    const int two_states_total = std::accumulate(
        _two_states_draft.begin(),
        _two_states_draft.end(),
        0
    );

    const int three_states_total = std::accumulate(
        _three_states_draft.begin(),
        _three_states_draft.end(),
        0
    );

    if (two_states_total != 100 || three_states_total != 100) {
        _randomization_invalid = true;

        _notifications.warning(
            "Simulation Settings",
            "Randomization distributions must sum to 100%."
        );

        return;
    }

    _settings.randomization.two_states = {
        static_cast<std::uint32_t>(_two_states_draft[0]),
        static_cast<std::uint32_t>(_two_states_draft[1])
    };

    _settings.randomization.three_states = {
        static_cast<std::uint32_t>(_three_states_draft[0]),
        static_cast<std::uint32_t>(_three_states_draft[1]),
        static_cast<std::uint32_t>(_three_states_draft[2])
    };

    _settings.randomization.seed = _random_seed_draft;

    _randomization_invalid = false;

    _notifications.info(
        "Simulation Settings",
        "Randomization settings updated successfully."
    );
}

void settings_window::render_randomization() {
    ImGui::Text("Randomization");
    ImGui::Separator();

    const int two_states_total =
        render_randomization_distribution(
            "Two-state automata",
            "TwoStates",
            _two_states_draft.data(),
            static_cast<int>(_two_states_draft.size())
        );

    ImGui::Spacing();
    ImGui::Separator();

    const int three_states_total =
        render_randomization_distribution(
            "Three-state automata",
            "ThreeStates",
            _three_states_draft.data(),
            static_cast<int>(_three_states_draft.size())
        );

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Random Seed");

    ImGui::SetNextItemWidth(-1.0f);

    ImGui::InputScalar(
        "##RandomSeed",
        ImGuiDataType_U32,
        &_random_seed_draft
    );

    ImGui::TextDisabled(
        "The same seed and configuration produce the same initial state."
    );

    const bool valid =
        two_states_total == 100 &&
        three_states_total == 100;

    if (valid)
        _randomization_invalid = false;

    ImGui::Spacing();
    ImGui::Separator();

    if (_randomization_invalid) {
        ImGui::TextColored(
            ImVec4(1.0f, 0.35f, 0.35f, 1.0f),
            "Each distribution must sum to exactly 100%%."
        );
    }

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float available_width = ImGui::GetContentRegionAvail().x;
    const float button_width = (available_width - spacing * 2.0f) / 3.0f;

    if (ImGui::Button(
        "Accept",
        ImVec2(button_width, 0.0f)
    )) {
        apply_randomization();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "Reset",
        ImVec2(button_width, 0.0f)
    )) {
        reset_randomization_draft();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "Default",
        ImVec2(button_width, 0.0f)
    )) {
        default_randomization_draft();
    }
}

void settings_window::default_interface_draft() {
    _interface_draft = interface_settings{};
}

void settings_window::reset_interface_draft() {
    _interface_draft = _settings.interface;
}

void settings_window::apply_interface_draft() {
    _settings.interface = _interface_draft;

    apply_interface_style(_settings.interface);

    _notifications.info(
        "Appearance Settings",
        "Interface settings updated successfully."
    );
}

void settings_window::render() {
    if (!_is_open)
        return;

    if (!ImGui::Begin("Settings", &_is_open)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("##SettingsTabs")) {
        const auto requested_tab = _active_tab;
        const bool select_requested_tab = _select_requested_tab;

        if (ImGui::BeginTabItem(
            "General", nullptr,
            select_requested_tab && requested_tab == settings_tab::general
            ? ImGuiTabItemFlags_SetSelected
            : ImGuiTabItemFlags_None)) {
            if (!select_requested_tab)
                _active_tab = settings_tab::general;

            render_general();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(
            "Simulation", nullptr,
            select_requested_tab && requested_tab == settings_tab::simulation
            ? ImGuiTabItemFlags_SetSelected
            : ImGuiTabItemFlags_None)) {
            if (!select_requested_tab)
                _active_tab = settings_tab::simulation;

            render_simulation();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(
            "Appearance", nullptr,
            select_requested_tab && requested_tab == settings_tab::appearance
            ? ImGuiTabItemFlags_SetSelected
            : ImGuiTabItemFlags_None)) {
            if (!select_requested_tab)
                _active_tab = settings_tab::appearance;

            render_appearance();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(
            "View", nullptr,
            select_requested_tab && requested_tab == settings_tab::view
            ? ImGuiTabItemFlags_SetSelected
            : ImGuiTabItemFlags_None)) {
            if (!select_requested_tab)
                _active_tab = settings_tab::view;

            render_view();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
        _select_requested_tab = false;
    }

    ImGui::End();
}



void settings_window::render_general() {
    // Workspace
    ImGui::Text("Workspace");

    ImGui::InputText(
        "Default filename",
        _workspace_filename_buffer.data(),
        _workspace_filename_buffer.size()
    );

    ImGui::TextDisabled(
        "Used as the default filename when saving a workspace."
    );

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        std::filesystem::path filename(
            _workspace_filename_buffer.data()
        );

        if (!filename.empty() &&
            filename.filename() == filename &&
            filename != "." &&
            filename != "..") {

            if (filename.extension().empty())
                filename += ".json";

            _settings.workspace_filename = filename.string();

            const auto normalized = _settings.workspace_filename;
            const auto length = std::min(
                normalized.size(),
                _workspace_filename_buffer.size() - 1
            );

            std::memcpy(
                _workspace_filename_buffer.data(),
                normalized.data(),
                length
            );

            _workspace_filename_buffer[length] = '\0';
        }
        else {
            const auto& previous = _settings.workspace_filename;
            const auto length = std::min(
                previous.size(),
                _workspace_filename_buffer.size() - 1
            );

            std::memcpy(
                _workspace_filename_buffer.data(),
                previous.data(),
                length
            );

            _workspace_filename_buffer[length] = '\0';
        }
    }

    ImGui::Spacing();

    ImGui::Text("Default save directory");

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float button_width = 90.0f;

    ImGui::SetNextItemWidth(
        ImGui::GetContentRegionAvail().x -
        button_width -
        spacing
    );

    ImGui::InputText(
        "##DefaultSaveDirectory",
        _save_directory_buffer.data(),
        _save_directory_buffer.size()
    );

    const bool directory_edited =
        ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SameLine();

    if (ImGui::Button(
        "Browse...",
        ImVec2(button_width, 0.0f)
    )) {
        _dialog_service.open_dialog(
            files::file_operation::select_save_directory
        );
    }

    if (directory_edited) {
        _settings.default_save_directory =
            _save_directory_buffer.data();
    }

    ImGui::TextDisabled(
        "Suggested directory when saving a new workspace."
    );

    ImGui::Separator();

    // Startup
    ImGui::Text("Startup");

    ImGui::Checkbox(
        "Load last workspace on startup",
        &_settings.load_workspace_on_startup
    );

    ImGui::TextDisabled(
        "Automatically loads the last successfully opened workspace."
    );

    ImGui::Separator();

    // Persistence
    ImGui::Text("Persistence");

    ImGui::Checkbox(
        "Save workspace on exit",
        &_settings.save_workspace_on_exit
    );

    ImGui::TextDisabled(
        "Automatically saves the current workspace when CAS closes."
    );

    ImGui::Checkbox(
        "Save interface layout",
        &_settings.save_imgui_layout
    );

    ImGui::TextDisabled(
        "Preserves the arrangement of panels between sessions."
    );
}

void settings_window::render_simulation() {
    render_randomization();

    ImGui::Text("Boundary");
    ImGui::Separator();

    if (!_view) {
        ImGui::TextDisabled("No viewport is available.");
        return;
    }

    const automata::boundary_type boundary =
        _view->get().simulation().automaton().topology().boundary();

    bool finite = boundary == automata::boundary_type::finite;
    bool toroidal = boundary == automata::boundary_type::toroidal;

    if (ImGui::RadioButton("Finite", finite)) {
        _view->get().simulation().set_boundary(
            automata::boundary_type::finite
        );
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Toroidal", toroidal)) {
        _view->get().simulation().set_boundary(
            automata::boundary_type::toroidal
        );
    }
}

void settings_window::render_appearance_preview() {
    ImGui::Text("Preview");
    ImGui::TextDisabled(
        "A preview of the CAS interface using the selected style."
    );

    interface_preview_scope preview_scope(_interface_draft);

    const float preview_height = 300.0f;

    if (!ImGui::BeginChild(
        "CASAppearancePreview",
        ImVec2(0.0f, preview_height),
        true
    )) {
        ImGui::EndChild();
        return;
    }

    // Simulated application menu.
    ImGui::Text("CAS - Cellular Automaton Simulator");
    ImGui::Separator();

    ImGui::BeginDisabled();

    ImGui::Text("File");
    ImGui::SameLine();
    ImGui::Text("View");
    ImGui::SameLine();
    ImGui::Text("Settings");

    ImGui::EndDisabled();

    ImGui::Separator();

    const ImVec2 available = ImGui::GetContentRegionAvail();
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float left_width = available.x * 0.28f;
    const float right_width = available.x - left_width - spacing;

    // Simulation panel.
    ImGui::BeginChild(
        "PreviewSimulation",
        ImVec2(left_width, 0.0f),
        true
    );

    ImGui::Text("Simulation");
    ImGui::Separator();

    ImGui::BeginDisabled();

    if (ImGui::Button("Start", ImVec2(-1.0f, 0.0f)))
    {
    }

    if (ImGui::Button("Step", ImVec2(-1.0f, 0.0f)))
    {
    }

    if (ImGui::Button("Randomize", ImVec2(-1.0f, 0.0f)))
    {
    }

    ImGui::Spacing();

    float speed = 30.0f;
    ImGui::Text("Simulation Speed");
    ImGui::SliderFloat(
        "##PreviewSpeed",
        &speed,
        1.0f,
        60.0f
    );

    ImGui::EndDisabled();

    ImGui::EndChild();

    ImGui::SameLine();

    // Viewport and properties.
    ImGui::BeginChild(
        "PreviewWorkspace",
        ImVec2(right_width, 0.0f),
        true
    );

    if (ImGui::BeginTabBar("PreviewViewports")) {
        if (ImGui::BeginTabItem("Viewport 1")) {
            ImGui::TextDisabled("100 x 100");
            ImGui::Separator();

            const ImVec2 size = ImGui::GetContentRegionAvail();
            const ImVec2 pos = ImGui::GetCursorScreenPos();

            ImDrawList* draw = ImGui::GetWindowDrawList();

            constexpr int rows = 6;
            constexpr int columns = 8;
            const float radius = 9.0f * _interface_draft.scale;

            const float hex_width = radius * 2.0f;
            const float hex_height = std::sqrt(3.0f) * radius;

            const float spacing_x = hex_width * 0.75f;
            const float spacing_y = hex_height;

            const float origin_x =
                pos.x + (size.x - (columns - 1) * spacing_x) * 0.5f;

            const float origin_y = pos.y + 12.0f;

            for (int r = 0; r < rows; ++r) {
                for (int q = 0; q < columns; ++q) {
                    const float x =
                        origin_x + q * spacing_x
                        + (r % 2) * spacing_x * 0.5f;

                    const float y = origin_y + r * spacing_y;

                    const bool alive =
                        (q == 3 && r == 2) ||
                        (q == 4 && r == 3) ||
                        (q == 2 && r == 3) ||
                        (q == 5 && r == 4);

                    ImVec2 points[6];

                    for (int i = 0; i < 6; ++i) {
                        const float angle = IM_PI / 3.0f * i;

                        points[i] = {
                            x + std::cos(angle) * radius,
                            y + std::sin(angle) * radius
                        };
                    }

                    if (alive) {
                        draw->AddConvexPolyFilled(
                            points,
                            6,
                            ImGui::GetColorU32(
                                ImGuiCol_ButtonActive
                            )
                        );
                    }

                    draw->AddPolyline(
                        points,
                        6,
                        ImGui::GetColorU32(
                            ImGuiCol_Border
                        ),
                        ImDrawFlags_Closed,
                        1.0f
                    );
                }
            }

            ImGui::Dummy(ImVec2(0.0f, size.y));

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();

    ImGui::Text("Properties");

    ImGui::BeginDisabled();

    float cell_size = 12.0f;
    ImGui::SliderFloat(
        "Cell Size",
        &cell_size,
        1.0f,
        50.0f
    );

    ImGui::EndDisabled();

    ImGui::EndChild();

    ImGui::EndChild();
}

void settings_window::render_appearance() {
    if (!ImGui::BeginChild(
        "AppearanceSettingsContent",
        ImVec2(0.0f, 0.0f),
        false
    )) {
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Interface");
    ImGui::Separator();

    constexpr const char* themes[] = {
        "Dark",
        "Light",
        "Classic"
    };

    constexpr const char* densities[] = {
        "Compact",
        "Comfortable",
        "Spacious"
    };

    int theme = static_cast<int>(_interface_draft.theme);

    if (ImGui::Combo(
        "Theme",
        &theme,
        themes,
        IM_ARRAYSIZE(themes)
    )) {
        _interface_draft.theme = static_cast<ui_theme>(theme);
    }

    int density = static_cast<int>(_interface_draft.density);

    if (ImGui::Combo(
        "UI Density",
        &density,
        densities,
        IM_ARRAYSIZE(densities)
    )) {
        _interface_draft.density =
            static_cast<ui_density>(density);
    }

    float scale_percent = _interface_draft.scale * 100.0f;

    ImGui::SetNextItemWidth(180.0f);

    if (ImGui::SliderFloat(
        "UI Scale",
        &scale_percent,
        80.0f,
        120.0f,
        "%.0f%%",
        ImGuiSliderFlags_AlwaysClamp
    )) {
        _interface_draft.scale = scale_percent / 100.0f;
    }

    ImGui::Spacing();
    ImGui::Separator();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float available_width = ImGui::GetContentRegionAvail().x;
    const float button_width = (available_width - spacing * 2.0f) / 3.0f;

    if (ImGui::Button(
        "Accept",
        ImVec2(button_width, 0.0f)
    )) {
        apply_interface_draft();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "Reset",
        ImVec2(button_width, 0.0f)
    )) {
        reset_interface_draft();
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "Default",
        ImVec2(button_width, 0.0f)
    )) {
        default_interface_draft();
    }

    ImGui::Spacing();
    ImGui::Separator();

    render_appearance_preview();

    ImGui::EndChild();
}
void settings_window::render_view() {
    ImGui::Text("Stats Panel");
    ImGui::Separator();

    ImGui::Checkbox(
        "Performance",
        &_settings.stats.show_performance
    );

    ImGui::Checkbox(
        "Memory",
        &_settings.stats.show_memory
    );

    ImGui::Checkbox(
        "Simulation",
        &_settings.stats.show_simulation
    );

    ImGui::Checkbox(
        "GPU",
        &_settings.stats.show_gpu
    );
}

} // namespace cas
