#include "application.h"	

#include <print>
#include <core/time.h>
#include <core/input.h>


namespace cas {

Application::Application()
	: m_automaton(50, 40) {

	m_window = std::make_unique<platform::Window>(
		"CAS - Cellular Automaton Simulator",
		800,
		600
	);

	m_renderer = std::make_unique<platform::Renderer>(
		m_window->native()
	);

	m_gui.init(
		m_window->native(),
		m_renderer->native()
	);
	m_automaton.randomize();

	m_is_running = true;
}

Application::~Application() {
	m_gui.shutdown();
}

void Application::run() {
	auto& time = cas::Time::instance();	
	auto& input = cas::Input::instance();

	while (m_is_running) {
		time.tick();	
		input.update();

		auto delta_time = time.delta_time();

		process_events();
		update(delta_time);
		render();
	}
}

void Application::process_events() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		m_gui.process_event(event);

		if (event.type == SDL_EVENT_QUIT)
			m_is_running = false;

		if (event.type == SDL_EVENT_KEY_DOWN) {
			const auto sc = event.key.scancode;

			if (sc == SDL_SCANCODE_ESCAPE)
				m_is_running = false;

			if (sc == SDL_SCANCODE_SPACE)
				m_is_simulating = !m_is_simulating;

			if (sc == SDL_SCANCODE_R)
				m_automaton.randomize();
		}

	}
}

void Application::update(double dt) {
	auto& time = Time::instance();

	if (!m_is_simulating) return;

	if (time.every(0.1, 0)) // 10 updates por segundo
		m_automaton.step();
	
}

void Application::render() {
	m_renderer->clear(20, 20, 20, 255);

	// ===== Dibujar automata =====

	const float size = 8.0f;
	const float offset_x = 50.0f;
	const float offset_y = 50.0f;

	m_renderer->set_draw_color(60, 60, 60, 255);

	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			float x = size * std::sqrt(3.0f) * (q + r * 0.5f);
			float y = size * 1.5f * r;

			m_renderer->draw_hex(
				x + offset_x,
				y + offset_y,
				size
			);
		}
	}

	m_renderer->set_draw_color(203, 203, 203, 255);

	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			const auto& cell = m_automaton.cell(q, r);
			if (!cell.alive)
				continue;

			float x = size * std::sqrt(3.0f) * (q + r * 0.5f);
			float y = size * 1.5f * r;

			m_renderer->fill_hex(
				x + offset_x,
				y + offset_y,
				size
			);
		}
	}

	// ===== GUI =====

	m_gui.begin_frame();

	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);

	bool open = true;
	ImGui::Begin("Simulation", &open);

	ImGui::Text("FPS: %.1f", 1.0 / Time::instance().delta_time());

	if (ImGui::Button(m_is_simulating ? "Pause" : "Start"))
		m_is_simulating = !m_is_simulating;

	if (ImGui::Button("Randomize"))
		m_automaton.randomize();

	ImGui::Text("SPACE: Start/Pause");
	ImGui::Text("R: Randomize");
	ImGui::Text("ESC: Exit");

	ImGui::End();

	m_gui.end_frame(m_renderer->native());

	m_renderer->present();
}


} // namespace cas