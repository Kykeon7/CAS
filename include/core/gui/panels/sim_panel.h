#pragma once

#include <core/gui/panels/gui_panel.h>
#include <core/viewport.h>

namespace cas::panels {

class Simulation : public Panel {
public:
	Simulation(Viewport& v) : m_viewport(v) {}

	void render() override {
		auto& sim = m_viewport.get().simulation();

		ImGui::Text("Simulation");

		ImGui::PushItemWidth(-1);

		ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());

		render_controls(sim);
		render_size(sim);
		render_speed(sim);
		render_rule(sim);

		ImGui::EndChild();

		ImGui::PopItemWidth();
	}

	void render_controls(cas::Simulation& sim) {

		if (ImGui::Button(sim.is_running() ? "Pause" : "Start"))
			sim.toggle();

		ImGui::SameLine();

		if (ImGui::Button("Step"))
			sim.step();

		ImGui::SameLine();

		if (ImGui::Button("Randomize"))
			sim.randomize();
	}

	void render_size(cas::Simulation& sim) {
		int width = sim.width();
		int height = sim.height();

		if (ImGui::InputInt("Width", &width) ||
			ImGui::InputInt("Height", &height))
		{
			width = std::max(1, width);
			height = std::max(1, height);

			sim.resize(width, height);
		}
	}

	void render_speed(cas::Simulation& sim) {
		float speed = sim.speed();

		if (ImGui::SliderFloat("Steps / sec", &speed, 1.0f, 60.0f))
			sim.set_speed(speed);
	}

	void render_rule(cas::Simulation& sim) {
		ImGui::Text(m_buffer);

		if (ImGui::Button("Edit Rule")) {
			open_rule_editor(sim);
		}

		render_rule_popup(sim);
	}

	void open_rule_editor(cas::Simulation& sim) {
		m_temp_rule = sim.rule();

		auto text = rules::rule_to_string(m_temp_rule);
		strncpy_s(m_buffer, text.c_str(), sizeof(m_buffer));

		ImGui::OpenPopup("Rule Editor");
	}

	void render_rule_popup(cas::Simulation& sim) {
		if (!ImGui::BeginPopupModal("Rule Editor"))
			return;

		render_rule_presets();
		render_rule_string();
		render_rule_checkboxes();

		render_rule_actions(sim);

		ImGui::EndPopup();
	}

	void render_rule_presets() {
		if (ImGui::Button("Game of Life"))
			m_temp_rule = rules::GameOfLife();

		ImGui::SameLine();

		if (ImGui::Button("HighLife"))
			m_temp_rule = rules::HighLife();

		ImGui::SameLine();

		if (ImGui::Button("Seeds"))
			m_temp_rule = rules::Seeds();

		if (ImGui::Button("Beehive"))
			return;

		if (ImGui::Button("Spiral"))
			return;
	}

	void render_rule_string() {
		if (ImGui::InputText("Rule", m_buffer, sizeof(m_buffer))) {
			m_temp_rule = rules::string_to_rule(m_buffer);
		}
	}

	void render_rule_checkboxes() {
		ImGui::Text("Birth:");
		for (int i = 0; i < 9; i++) {
			std::string label = std::to_string(i) + "##B" + std::to_string(i);

			if (ImGui::Checkbox(label.c_str(), &m_temp_rule.birth[i]))
				sync_rule_buffer();

			if (i != 8) ImGui::SameLine();
		}

		ImGui::Text("Survive:");
		for (int i = 0; i < 9; i++) {
			std::string label = std::to_string(i) + "##S" + std::to_string(i);

			if (ImGui::Checkbox(label.c_str(), &m_temp_rule.survive[i]))
				sync_rule_buffer();

			if (i != 8) ImGui::SameLine();
		}
	}

	void sync_rule_buffer() {
		auto text = rules::rule_to_string(m_temp_rule);
		strncpy_s(m_buffer, text.c_str(), sizeof(m_buffer));
	}

	void render_rule_actions(cas::Simulation& sim) const {
		if (ImGui::Button("Apply")) {
			sim.set_rule(m_temp_rule);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
	}

	void set_viewport(Viewport& vp) override {
		m_viewport = vp;
	}

private:
	std::reference_wrapper<Viewport> m_viewport;

	rule m_temp_rule;
	char m_buffer[32] = "B3/S32";
	bool m_initialized = false;
};

} // namespace cas::panels