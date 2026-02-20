#include "application.h"	

#include <print>
#include <core/time.h>
#include <core/input.h>
#include <algorithm>


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

	m_renderer->set_camera(&m_camera);

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
	const float speed = 1500.0f * Time::instance().delta_time();

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

			if (sc == SDL_SCANCODE_F11)
				m_window->set_full_screen(!m_window->is_full_screen());

			if (sc == SDL_SCANCODE_W) m_camera.y -= speed;
			if (sc == SDL_SCANCODE_S) m_camera.y += speed;
			if (sc == SDL_SCANCODE_A) m_camera.x -= speed;
			if (sc == SDL_SCANCODE_D) m_camera.x += speed;

		}

		if (event.type == SDL_EVENT_MOUSE_WHEEL) {
			float mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);

			// Coordenadas mundo antes del zoom
			float world_x = mouse_x / m_camera.zoom + m_camera.x;
			float world_y = mouse_y / m_camera.zoom + m_camera.y;

			float zoom_factor = 1.1f;

			if (event.wheel.y > 0)
				m_camera.zoom *= zoom_factor;
			else if (event.wheel.y < 0)
				m_camera.zoom /= zoom_factor;

			m_camera.zoom = std::clamp(m_camera.zoom, 0.1f, 10.0f);

			// Reajustar cámara para centrar zoom en cursor
			m_camera.x = world_x - mouse_x / m_camera.zoom;
			m_camera.y = world_y - mouse_y / m_camera.zoom;
		}
	}
}

void Application::update(double dt) {
	auto& time = Time::instance();

	if (!m_is_simulating) return;

	if (time.every(1 / m_simulation_speed, 0))
		m_automaton.step();
	
}

void Application::render() {
	m_renderer->clear(
		static_cast<Uint8>(m_background_color.x * 255),
		static_cast<Uint8>(m_background_color.y * 255),
		static_cast<Uint8>(m_background_color.z * 255),
		255
	);


	// ===== Dibujar automata =====

	const float size = 8.0f;
	const float offset_x = 50.0f;
	const float offset_y = 50.0f;
	const float sqrt3 = std::sqrt(3.0f);

	m_renderer->set_draw_color(
		static_cast<Uint8>(m_grid_color.x * 255),
		static_cast<Uint8>(m_grid_color.y * 255),
		static_cast<Uint8>(m_grid_color.z * 255),
		255
	);


	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			float x = size * sqrt3 * (q + r * 0.5f);
			float y = size * 1.5f * r;

			m_renderer->draw_hex(
				x + offset_x,
				y + offset_y,
				size
			);
		}
	}

	m_renderer->set_draw_color(
		static_cast<Uint8>(m_alive_color.x * 255),
		static_cast<Uint8>(m_alive_color.y * 255),
		static_cast<Uint8>(m_alive_color.z * 255),
		255
	);

	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			const auto& cell = m_automaton.cell(q, r);
			if (!cell.alive)
				continue;

			float x = size * sqrt3 * (q + r * 0.5f);
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

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(320, 260), ImGuiCond_Always);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize;

	ImGui::Begin("Simulation", nullptr, flags);

	// ===== FPS =====

	ImGui::Text("FPS: %.1f", 1.0 / Time::instance().delta_time());
	ImGui::Separator();

	// ===== Control de simulacion =====

	if (ImGui::Button(m_is_simulating ? "Pause" : "Start"))
		m_is_simulating = !m_is_simulating;

	ImGui::SameLine();

	if (ImGui::Button("Randomize"))
		m_automaton.randomize();

	ImGui::SliderFloat(
		"Steps / sec",
		&m_simulation_speed,
		1.0f,
		60.0f,
		"%.1f"
	);

	ImGui::Separator();

	// ===== Color =====

	ImGui::Separator();
	ImGui::Text("Appearance");

	ImGui::ColorEdit3("Alive Color", (float*)&m_alive_color);
	ImGui::ColorEdit3("Grid Color", (float*)&m_grid_color);
	ImGui::ColorEdit3("Background Color", (float*)&m_background_color);

	// ===== Estadisticas =====

	ImGui::Text("Generation: %d", m_automaton.generation_count());
	ImGui::Text("Alive cells: %d", m_automaton.alive_count());

	ImGui::Separator();

	// ===== Atajos =====

	ImGui::Text("SPACE: Start/Pause");
	ImGui::Text("R: Randomize");
	ImGui::Text("ESC: Exit");

	ImGui::End();

	m_gui.end_frame(m_renderer->native());

	m_renderer->present();
}

} // namespace cas