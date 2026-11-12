#include <gui/rule_editor.h>

#include <imgui.h>

#include <cstdio>
#include <string>
#include <utility>

#include <automata/rules/binary.h>

namespace cas::gui {

void rule_editor::open(cas::simulations::simulation& sim) {
    const auto& current = sim.rule();

    if (const auto* binary =
        dynamic_cast<const automata::rules::binary*>(&current)) {
        _temp_rule = std::make_unique<automata::rules::binary>(
            binary->birth(),
            binary->survive()
        );
    }
    else if (dynamic_cast<const class automata::rules::spiral*>(&current)) {
        _temp_rule = std::make_unique<class automata::rules::spiral>();
    }
    else {
        _temp_rule = automata::rules::string_to_rule(
            automata::rules::rule_to_string(current)
        );
    }

    sync_rule_buffer();
    ImGui::OpenPopup("Rule Editor");
}

void rule_editor::render(cas::simulations::simulation& sim) {
    render_popup(sim);
}

std::string rule_editor::rule_display_name(
    const automata::rules::rule& current
) const {
    if (dynamic_cast<const class automata::rules::spiral*>(&current)) {
        return "Spiral";
    }

    if (dynamic_cast<const class automata::rules::rule_22_majority_memory*>(&current)) {
        return "Rule 22 (Majority Memory)";
    }

    if (const auto* binary =
        dynamic_cast<const automata::rules::binary*>(&current)) {
        const auto notation = automata::rules::rule_to_string(*binary);

        if (notation == "B3/S23") {
            return "Game of Life";
        }

        if (notation == "B36/S23") {
            return "HighLife";
        }

        if (notation == "B2/S") {
            return "Seeds";
        }

        if (notation == "B3/S012345678") {
            return "Beehive";
        }

        return "Custom Binary Rule";
    }

    return "Custom Rule";
}

void rule_editor::render_popup(cas::simulations::simulation& sim) {
    if (!ImGui::BeginPopupModal(
        "Rule Editor",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
    )) {
        return;
    }

    render_presets();
    ImGui::Separator();

    render_string();
    render_checkboxes();

    ImGui::Separator();

    render_actions(sim);

    ImGui::EndPopup();
}

void rule_editor::render_presets() {
    ImGui::Text("Presets");

    if (ImGui::Button("Life")) {
        set_temp_rule(automata::rules::game_of_life());
    }

    ImGui::SameLine();

    if (ImGui::Button("HighLife")) {
        set_temp_rule(automata::rules::high_life());
    }

    ImGui::SameLine();

    if (ImGui::Button("Seeds")) {
        set_temp_rule(automata::rules::seeds());
    }

    ImGui::SameLine();

    if (ImGui::Button("Beehive")) {
        set_temp_rule(automata::rules::beehive());
    }

    if (ImGui::Button("Spiral")) {
        set_temp_rule(automata::rules::spiral());
    }

    ImGui::SameLine();

    if (ImGui::Button("R22maj:4")) {
        set_temp_rule(automata::rules::rule_22_majority_memory());
    }
}

void rule_editor::render_string() {
    ImGui::Text("Rule notation");

    if (ImGui::InputText(
        "##rule_notation",
        _rule_buffer,
        sizeof(_rule_buffer)
    )) {
        auto new_rule = automata::rules::string_to_rule(_rule_buffer);

        if (new_rule)
            _temp_rule = std::move(new_rule);
        else
            _temp_rule.reset();
    }

    if (!_temp_rule) {
        ImGui::TextColored(
            ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
            "Invalid rule notation."
        );
    }
}

void rule_editor::render_checkboxes() {
    auto* binary = dynamic_cast<automata::rules::binary*>(_temp_rule.get());

    if (!binary) {
        return;
    }

    ImGui::Separator();
    ImGui::Text("Birth");

    for (int i = 0; i < 9; ++i) {
        bool value = binary->birth()[i];

        ImGui::PushID(i);

        if (ImGui::Checkbox(
            std::to_string(i).c_str(),
            &value
        )) {
            binary->birth()[i] = value;
            sync_rule_buffer();
        }

        ImGui::PopID();

        if (i != 8) {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();
    ImGui::Text("Survive");

    for (int i = 0; i < 9; ++i) {
        bool value = binary->survive()[i];

        ImGui::PushID(i + 9);

        if (ImGui::Checkbox(
            std::to_string(i).c_str(),
            &value
        )) {
            binary->survive()[i] = value;
            sync_rule_buffer();
        }

        ImGui::PopID();

        if (i != 8) {
            ImGui::SameLine();
        }
    }
}

void rule_editor::render_actions(cas::simulations::simulation& sim) {
    if (ImGui::Button("Apply")) {
        if (_temp_rule) {
            sim.set_rule(std::move(_temp_rule));
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        _temp_rule.reset();
        ImGui::CloseCurrentPopup();
    }
}

void rule_editor::sync_rule_buffer() {
    if (!_temp_rule) {
        return;
    }

    const auto notation = automata::rules::rule_to_string(*_temp_rule);

    std::snprintf(
        _rule_buffer,
        sizeof(_rule_buffer),
        "%s",
        notation.c_str()
    );
}

void rule_editor::set_temp_rule(
    std::unique_ptr<automata::rules::rule> new_rule
) {
    _temp_rule = std::move(new_rule);
    sync_rule_buffer();
}

}