#pragma once

#include <core/gui/panels/gui_panel.h>
#include <core/time.h>
#include <system/memory.h>
#include <system/profiler.h>

namespace cas::panels {

class Stats : public Panel {
public:
	Stats(const automaton& a) : m_automaton(a) {}
	void render() override {
		auto& profiler = system::Profiler::instance();

		ImGui::Text("Stats");

		ImGui::PushItemWidth(-1);

		ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());


		ImGui::PushItemWidth(-1);

		ImGui::Text("FPS: %.1f", 1.0 / Time::instance().delta_time());
		ImGui::Text("Memory: %.2f MB", system::get_memory_usage_mb());

		ImGui::Text("Frame Time: %.2f ms", profiler.get_ms("run"));
		ImGui::Text("Update: %.2f ms", profiler.get_ms("update"));
		ImGui::Text("Render: %.2f ms", profiler.get_ms("render"));
		ImGui::Separator();

		ImGui::Text("Generation: %d", m_automaton.generation_count());
		ImGui::Text("Alive cells: %d", m_automaton.alive_count());


		ImGui::EndChild();

		ImGui::PopItemWidth();
	}

private:
	const automaton& m_automaton;
};

} // namespace cas::panels