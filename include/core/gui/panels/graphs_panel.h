#pragma once

#include <core/gui/panels/gui_panel.h>
#include <core/viewport.h>

namespace cas::panels {

class Graphs : public Panel {
public:
	Graphs(Viewport& viewport, float& ui_scale)
		: m_viewport(viewport), m_ui_scale(ui_scale)
	{}

    void render() override {
        ImGui::Text("Graphs");

        ImGui::PushItemWidth(-1);

        ImGui::BeginChild("GraphsContent", ImGui::GetContentRegionAvail());

        render_population();
        ImGui::Separator();

        render_density();
        ImGui::Separator();

        render_activity();

        ImGui::EndChild();

        ImGui::PopItemWidth();
    }

    void render_population() {
        auto& history = m_viewport.get().population_history();
        auto& automaton = m_viewport.get().simulation().get_automaton();

        if (history.empty()) return;

        ImGui::Text("Population");

        ImGui::PlotLines(
            "##Population",
            history.data(),
            static_cast<int>(history.size()),
            0,
            nullptr,
            0.0f,
            static_cast<float>(automaton.width() * automaton.height()),
            ImVec2(0, 80 * m_ui_scale)
        );
    }

    void render_density() {
        auto& history = m_viewport.get().density_history();

        if (history.empty()) return;

        ImGui::Text("Density");

        ImGui::PlotLines(
            "##Density",
            history.data(),
            static_cast<int>(history.size()),
            0,
            nullptr,
            0.0f,
            1.0f,
            ImVec2(0, 80 * m_ui_scale)
        );
    }

    void render_activity() {
        auto& history = m_viewport.get().activity_history();

        if (history.empty()) return;

        ImGui::Text("Activity");

        ImGui::PlotLines(
            "##Activity",
            history.data(),
            static_cast<int>(history.size()),
            0,
            nullptr,
            0.0f,
            FLT_MAX,
            ImVec2(0, 80 * m_ui_scale)
        );
    }

	void set_viewport(Viewport& vp) {
		m_viewport = vp;
	}
private:
	float& m_ui_scale;
	std::reference_wrapper<Viewport> m_viewport;

};

} // namespace cas::panels