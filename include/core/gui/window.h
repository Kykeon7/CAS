#pragma once

#include <core/gui/panels/gui_panel.h>

namespace cas {

class Window {
public:
	Window(const std::string& name)
		: m_name(name) 
	{}

	void add_panel(panels::Panel& panel) {
		m_panels.emplace_back(panel);
	}

	void render() {
		ImGui::Begin(m_name.c_str());

		for (size_t i = 0; i < m_panels.size(); ++i) {
			auto& panel = m_panels[i];

			panel.get().render();

			if (i + 1 != m_panels.size())
				ImGui::Separator();
		}

		ImGui::End();
	}

private:
	std::string m_name;
	std::vector <std::reference_wrapper<panels::Panel>> m_panels;

};

} // namespace cas