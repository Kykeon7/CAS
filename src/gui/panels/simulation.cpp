
#include <gui/panels/simulation.h>

#include <automata/rules/binary.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <utility>
#include <random>
#include <imgui.h>

namespace cas::gui::panels {


simulation::simulation(
    viewport_view& view,
    settings& settings,
    std::function<void(settings_tab)> open_settings
)
    : _view(view),
      _settings(settings),
      _open_settings(std::move(open_settings)) {
}


void simulation::render() {
    auto& sim = _view.simulation();

    ImGui::Text("Simulation");
    ImGui::Separator();

    ImGui::PushItemWidth(-1.0f);

    ImGui::BeginChild(
        "SimulationContent",
        ImGui::GetContentRegionAvail()
    );

    render_controls(sim);
    ImGui::Spacing();

    render_size(sim);
    render_speed(sim);
    render_rule(sim);

    ImGui::EndChild();

    ImGui::PopItemWidth();
}


void simulation::render_controls(cas::simulations::simulation& sim) {
    const ImVec2 button_size(-1.0f, 0.0f);

    if (ImGui::Button(
        sim.is_running() ? "Pause" : "Start",
        button_size
    )) {
        sim.toggle();
    }

    ImGui::BeginDisabled(sim.is_running());

    if (ImGui::Button("Step", button_size)) {
        sim.step();
    }

    ImGui::EndDisabled();

    if (ImGui::Button("Clear", button_size)) {
        sim.clean();
    }

    const auto seed = _settings.randomization.seed;
    const int states = sim.rule().state_count();

    // Custom distribution + configured seed.
    ImGui::BeginDisabled(states != 2 && states != 3);

    if (ImGui::Button("Randomize (Custom)", button_size)) {
        if (states == 2) {
            sim.randomize(
                _settings.randomization.two_states,
                seed
            );
        }
        else if (states == 3) {
            sim.randomize(
                _settings.randomization.three_states,
                seed
            );
        }
    }

    ImGui::EndDisabled();

    // Default distribution + configured seed.
    if (ImGui::Button("Randomize (Seed Only)", button_size)) {
        sim.randomize(seed);
    }

    // Default distribution + random seed.
    if (ImGui::Button("Randomize (Random Seed)", button_size)) {
        std::random_device random_device;
        sim.randomize(random_device());
    }

    if (ImGui::Button("Configuration", button_size)) {
        _open_settings(settings_tab::simulation);
    }
}

void simulation::render_size(cas::simulations::simulation& sim) {
    ImGui::Text("Grid Size");

    ImGui::TextDisabled(
        "%d x %d",
        sim.width(),
        sim.height()
    );

    if (ImGui::Button("Resize", ImVec2(-1.0f, 0.0f))) {
        _temp_width = sim.width();
        _temp_height = sim.automaton().topology().is_linear()
            ? 1
            : sim.height();

        ImGui::OpenPopup("Resize Grid");
    }

    render_resize_popup(sim);
}

void simulation::render_resize_popup(cas::simulations::simulation& sim) {
    if (!ImGui::BeginPopupModal(
        "Resize Grid",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
    )) {
        return;
    }

    const bool is_linear =
        sim.automaton().topology().is_linear();

    if (is_linear) {
        _temp_height = 1;
    }

    ImGui::Text("Set the grid dimensions.");
    ImGui::Spacing();

    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputInt("Width", &_temp_width);

    ImGui::BeginDisabled(is_linear);

    ImGui::SetNextItemWidth(220.0f);
    ImGui::InputInt("Height", &_temp_height);

    ImGui::EndDisabled();

    const int new_height = is_linear
        ? 1
        : std::max(1, _temp_height);

    ImGui::Spacing();

    ImGui::TextDisabled(
        "New size: %d x %d",
        std::max(1, _temp_width),
        new_height
    );

    ImGui::Separator();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float available_width = ImGui::GetContentRegionAvail().x;
    const float button_width = (available_width - spacing) / 2.0f;

    if (ImGui::Button(
        "Apply",
        ImVec2(button_width, 0.0f)
    )) {
        if (_temp_width > 0 && (is_linear || _temp_height > 0)) {
            sim.resize(_temp_width, new_height);
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button(
        "Cancel",
        ImVec2(button_width, 0.0f)
    )) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void simulation::render_speed(cas::simulations::simulation& sim) {
    float speed = sim.speed();

    ImGui::Text("Simulation Speed");
    ImGui::SetNextItemWidth(-1.0f);

    if (ImGui::SliderFloat(
        "##SimulationSpeed",
        &speed,
        1.0f,
        60.0f,
        "%.1f steps/sec"
    )) {
        sim.set_speed(speed);
    }
}

void simulation::render_rule(cas::simulations::simulation& sim) {
    ImGui::Text("Rule");
    ImGui::SameLine();

    const std::string name =
        _rule_editor.rule_display_name(sim.rule());

    ImGui::TextDisabled("%s", name.c_str());

    if (ImGui::Button("Edit Rule", ImVec2(-1.0f, 0.0f))) {
        _rule_editor.open(sim);
    }

    _rule_editor.render(sim);
}

} // namespace cas::gui::panels
