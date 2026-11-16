#include <gui/panels/patterns.h>

#include <imgui.h>

#include <string>

#include <automata/patterns.h>
#include <automata/experiment_presets.h>
#include <exception>
#include <utility>
#include <automata/rules/factory.h>
#include <gui/pattern_preview.h>

namespace cas::gui::panels {

patterns::patterns(viewport_view& view) : _view(view) {}

void patterns::render() {
    const auto& automaton = _view.get().simulation().automaton();
    const std::string rule = automata::rules::rule_to_string(automaton.rule());

    ImGui::TextUnformatted("Pattern Library");
    ImGui::Separator();
    ImGui::TextWrapped("Drag an available pattern onto the viewport. "
                       "Unavailable presets are listed below in a dimmed style.");

    const auto draw_entry = [&](const automata::patterns::descriptor& entry,
                                bool available) {
        ImGui::PushID(static_cast<int>(entry.identifier));
        const auto source = automata::patterns::create(entry.identifier);
        const ImVec2 start = ImGui::GetCursorScreenPos();
        const float width = ImGui::GetContentRegionAvail().x;
        const ImVec2 thumbnail{68.0f, 68.0f};
        if (!available)
            ImGui::BeginDisabled();
        ImGui::InvisibleButton("##pattern", {width, 78.0f});
        const bool hovered = available && ImGui::IsItemHovered();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddRectFilled(start, {start.x + width, start.y + 78.0f},
            hovered ? IM_COL32(95, 125, 155, 35) :
            available ? IM_COL32(95, 125, 155, 15) : IM_COL32(95, 125, 155, 8), 4.0f);
        draw_pattern_preview(draw, source, {start.x + 5.0f, start.y + 5.0f},
            thumbnail, entry.required_topology == automata::patterns::neighborhood::hexagonal,
            available ? IM_COL32(65, 170, 235, 255) : IM_COL32(120, 125, 135, 90),
            available ? IM_COL32(245, 180, 65, 255) : IM_COL32(145, 130, 100, 90));
        draw->AddText({start.x + 82.0f, start.y + 12.0f},
            available ? ImGui::GetColorU32(ImGuiCol_Text) :
                        IM_COL32(150, 150, 150, 120), entry.name.data());
        draw->AddText({start.x + 82.0f, start.y + 36.0f},
            available ? ImGui::GetColorU32(ImGuiCol_TextDisabled) :
                        IM_COL32(135, 135, 135, 100), entry.description.data());
        if (available && ImGui::BeginDragDropSource()) {
            const int id = static_cast<int>(entry.identifier);
            ImGui::SetDragDropPayload("CAS_PATTERN", &id, sizeof(id));
            ImGui::TextUnformatted(entry.name.data());
            const ImVec2 drag_origin = ImGui::GetCursorScreenPos();
            ImGui::Dummy({96.0f, 96.0f});
            draw_pattern_preview(ImGui::GetWindowDrawList(), source,
                drag_origin, {96.0f, 96.0f},
                entry.required_topology == automata::patterns::neighborhood::hexagonal);
            ImGui::EndDragDropSource();
        }
        if (!available)
            ImGui::EndDisabled();
        ImGui::PopID();
    };

    int available_count = 0;
    for (const auto& entry : automata::patterns::catalogue) {
        if (!automata::patterns::supports(entry.identifier, automaton.topology(), rule))
            continue;
        draw_entry(entry, true);
        ++available_count;
    }
    if (available_count == 0)
        ImGui::TextDisabled("No presets for the active rule and topology.");

    ImGui::SeparatorText("Unavailable");
    for (const auto& entry : automata::patterns::catalogue) {
        if (automata::patterns::supports(entry.identifier, automaton.topology(), rule))
            continue;
        draw_entry(entry, false);
    }

    ImGui::Separator();
    render_experiments();

    if (ImGui::CollapsingHeader("Tests", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped(
            "Regression tests verify known behavior in an isolated 32x32 "
            "Game of Life simulation. They do not modify your viewport."
        );
        if (ImGui::Button("Run pattern tests"))
            _results = automata::run_pattern_tests();

        for (const auto& result : _results) {
            ImGui::TextColored(
                result.passed ? ImVec4(0.3f, 0.9f, 0.4f, 1.0f) :
                    ImVec4(1.0f, 0.35f, 0.35f, 1.0f),
                "%s: %s", result.name.c_str(), result.passed ? "PASS" : "FAIL"
            );
            ImGui::TextWrapped("%s", result.detail.c_str());
        }
        ImGui::TextDisabled(
            "Block: stable after 1 step. Blinker: repeats after 2 steps. "
            "Glider: shifts (1,1) after 4 steps. Invalid states are rejected."
        );
    }
}


void patterns::render_experiments() {
    if (ImGui::CollapsingHeader("Experiments", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped(
            "Precomputed initial configurations. Opening one replaces the current "
            "automaton, including its dimensions, topology, rule and cell states."
        );
        ImGui::TextDisabled("These are visual experiments, not automatic logic-gate tests.");
        for (const auto& entry : automata::experiments::catalogue) {
            ImGui::PushID(static_cast<int>(entry.identifier));
            if (ImGui::Button("Open")) {
                _pending_experiment = static_cast<int>(entry.identifier);
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(entry.name.data());
            ImGui::TextDisabled("%dx%d | %s | %s", entry.width, entry.height,
                entry.hexagonal ? "Hexagonal" : "Moore", entry.rule.data());
            ImGui::TextWrapped("%s", entry.description.data());
            ImGui::PopID();
        }
    }

    if (_pending_experiment >= 0 && !ImGui::IsPopupOpen("Confirm experiment"))
        ImGui::OpenPopup("Confirm experiment");

    // Render the modal even when the section is collapsed.
    if (ImGui::BeginPopupModal("Confirm experiment", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("Replace the current automaton with this experiment?");
        ImGui::TextWrapped("All unsaved changes in the active automaton will be lost.");
        ImGui::Separator();
        if (ImGui::Button("Replace automaton")) {
            try {
                if (_pending_experiment >= 0 &&
                    _pending_experiment < static_cast<int>(automata::experiments::catalogue.size())) {
                    auto replacement = automata::experiments::create(
                        static_cast<automata::experiments::id>(_pending_experiment));
                    auto& sim = _view.simulation();
                    if (sim.is_running())
                        sim.toggle();
                    sim.automaton() = std::move(replacement);
                    sim.reset_timing();
                    _view.get().population_history().clear();
                    _view.get().density_history().clear();
                    _view.get().activity_history().clear();
                    _view.get().linear_history_data().clear();
                    _view.get().reset_position();
                    _view.get().zoom_reset();
                    _experiment_error.clear();
                }
            }
            catch (const std::exception& e) {
                _experiment_error = e.what();
            }
            _pending_experiment = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            _pending_experiment = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (!_experiment_error.empty())
        ImGui::TextWrapped("Could not open experiment: %s", _experiment_error.c_str());
}

} // namespace cas::gui::panels
