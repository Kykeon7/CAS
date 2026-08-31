#pragma once

#include <core/gui/panels/gui_panel.h>

namespace cas::panels {

class Help : public Panel {
public:
	Help() = default;
	void render() override {
		ImGui::Text("Help");

		ImGui::PushItemWidth(-1);

		ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());

		ImGui::Text("SPACE: Start/Pause");
		ImGui::Text("R: Randomize");
		ImGui::Text("ESC: Exit");

		ImGui::EndChild();

		ImGui::PopItemWidth();

	}
};

} // namespace cas::panels