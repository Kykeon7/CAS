#pragma once

#include <core/gui/panels/gui_panel.h>

namespace cas::panels {

	class File : public Panel {
	public:
		void render() override {

			ImGui::Text("File");

			// ===== Save =====
			if (ImGui::Button("Save"))
				ImGui::OpenPopup("Save Popup");

			ImGui::SameLine();

			// ===== Load =====
			if (ImGui::Button("Load"))
				ImGui::OpenPopup("Load Popup");

			render_save_popup();
			render_load_popup();
		}

	private:

		void render_save_popup() {

			if (!ImGui::BeginPopupModal(
				"Save Popup",
				nullptr,
				ImGuiWindowFlags_AlwaysAutoResize
			))
				return;

			ImGui::Text("Select what to save");
			ImGui::Separator();

			if (ImGui::Button("Save Grid")) {
				// TODO
			}

			if (ImGui::Button("Save Viewport")) {
				// TODO
			}

			if (ImGui::Button("Save Workspace")) {
				// TODO
			}

			ImGui::Separator();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		void render_load_popup() {

			if (!ImGui::BeginPopupModal(
				"Load Popup",
				nullptr,
				ImGuiWindowFlags_AlwaysAutoResize
			))
				return;

			ImGui::Text("Select what to load");
			ImGui::Separator();

			if (ImGui::Button("Load Grid")) {
				// TODO
			}

			if (ImGui::Button("Load Viewport")) {
				// TODO
			}

			if (ImGui::Button("Load Workspace")) {
				// TODO
			}

			ImGui::Separator();

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	};

} // namespace cas::panels