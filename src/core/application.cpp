#include "core/application.h"	

#include <print>
#include <core/time.h>
#include <core/input.h>
#include <algorithm>
#include "core/math/vec2.h"
#include <core/graphics/color_conv.h>


namespace cas {

Application::Application()
    : m_window(
          "CAS - Cellular Automaton Simulator",
          800,
          600
      )
    , m_renderer(m_window.native())
    , m_gui(m_window, m_renderer)
	, m_automaton(40, 50)
{

	m_renderer.set_camera(&m_camera);

	m_automaton.randomize();

	m_is_running = true;
}

void Application::run() {
	auto& time = cas::Time::instance();	
	auto& input = cas::Input::instance();

	update_imgui_scale();

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
	const float speed = static_cast<float>(1500.0f * Time::instance().delta_time());

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

			if (sc == SDL_SCANCODE_F11) {
				bool fs = !m_window.is_full_screen();
				m_window.set_full_screen(fs);

				m_ui_scale = fs ? 1.5f : 1.0f;

				ImGui::GetIO().FontGlobalScale = m_ui_scale;
			}

			Vec2 cam = m_camera.position();

			if (sc == SDL_SCANCODE_W) cam.y -= speed;
			if (sc == SDL_SCANCODE_S) cam.y += speed;
			if (sc == SDL_SCANCODE_A) cam.x -= speed;
			if (sc == SDL_SCANCODE_D) cam.x += speed;

			m_camera.set_position(cam);

		}

		if (event.type == SDL_EVENT_MOUSE_WHEEL) {
			float mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);

			// Coordenadas mundo antes del zoom
			float world_x = mouse_x / m_camera.zoom() + m_camera.position().x;
			float world_y = mouse_y / m_camera.zoom() + m_camera.position().y;

			float zoom_factor = 1.1f;

			if (event.wheel.y > 0)
				m_camera.set_zoom(m_camera.zoom() * zoom_factor);
			else if (event.wheel.y < 0)
				m_camera.set_zoom(m_camera.zoom() / zoom_factor);

			m_camera.set_zoom(std::clamp(m_camera.zoom(), 0.1f, 10.0f));

			// Reajustar cámara para centrar zoom en cursor
			Vec2 pos{
				world_x - mouse_x / m_camera.zoom(),
				world_y - mouse_y / m_camera.zoom()
			};

			m_camera.set_position(pos);
		}

		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureMouse)
				continue;

			if (event.button.button == SDL_BUTTON_LEFT) {
				float mouse_x = static_cast<float>(event.button.x);
				float mouse_y = static_cast<float>(event.button.y);

				// Convertir a mundo
				float world_x = mouse_x / m_camera.zoom() + m_camera.position().x;
				float world_y = mouse_y / m_camera.zoom() + m_camera.position().y;

				handle_cell_click(world_x, world_y);
			}
		}

	}
}

void Application::update(double dt) {
	auto& time = Time::instance();

	if (!m_is_simulating) return;

	if (time.every(1 / m_simulation_speed, 0)) {
		m_automaton.step();

		m_population_history.push_back(
			static_cast<float>(m_automaton.alive_count())
		);

		if (m_population_history.size() > 500)
			m_population_history.erase(m_population_history.begin());
	}
	
}

void Application::render() {
	m_renderer.clear(m_background_color);

	// ===== Dibujar automata =====

	const float size = 8.0f;
	const float offset_x = 50.0f;
	const float offset_y = 50.0f;
	const float sqrt3 = std::sqrt(3.0f);

	m_renderer.set_draw_color(m_grid_color);


	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			float x = size * sqrt3 * (q + r * 0.5f);
			float y = size * 1.5f * r;

			Vec2 hex_pos = {
				x + offset_x,
				y + offset_y
			};

			m_renderer.draw_hex(
				hex_pos,
				size
			);
		}
	}

	m_renderer.set_draw_color(m_alive_color);

	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			const auto& cell = m_automaton.cell(q, r);
			if (!cell.alive)
				continue;

			float x = size * sqrt3 * (q + r * 0.5f);
			float y = size * 1.5f * r;

			Vec2 fill{
				x + offset_x,
				y + offset_y
			};

			m_renderer.fill_hex(
				fill,
				size
			);
		}
	}

	// ===== GUI =====
	{
		auto frame = m_gui.begin_frame(m_renderer);

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(320 * m_ui_scale, 260 * m_ui_scale), ImGuiCond_Always);

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
		ImVec4 temp_color = to_imgui(m_alive_color);

		ImGui::Separator();
		ImGui::Text("Appearance");

		if (ImGui::ColorEdit3("Alive Color", (float*)&temp_color))
			m_alive_color = from_imgui(temp_color);

		temp_color = to_imgui(m_grid_color);
		if (ImGui::ColorEdit3("Grid Color", (float*)&temp_color))
			m_grid_color = from_imgui(temp_color);

		temp_color = to_imgui(m_background_color);
		if (ImGui::ColorEdit3("Background Color", (float*)&temp_color))
			m_background_color = from_imgui(temp_color);

		// ===== Estadisticas =====

		ImGui::Text("Generation: %d", m_automaton.generation_count());
		ImGui::Text("Alive cells: %d", m_automaton.alive_count());

		ImGui::Separator();

		// ===== Atajos =====

		ImGui::Text("SPACE: Start/Pause");
		ImGui::Text("R: Randomize");
		ImGui::Text("ESC: Exit");

		ImGui::Separator();
		ImGui::Text("Population over time");

		if (!m_population_history.empty()) {
			ImGui::PlotLines(
				"##Population",
				m_population_history.data(),
				static_cast<int>(m_population_history.size()),
				0,
				nullptr,
				0.0f,
				static_cast<float>(m_automaton.width() * m_automaton.height()),
				ImVec2(0, 80 * m_ui_scale)
			);
		}

		ImGui::End();
	}

	m_renderer.present();
}

void Application::update_imgui_scale() {
	float scale = SDL_GetWindowDisplayScale(m_window.native());

	if (scale <= 0.0f)
		scale = 1.0f;

	ImGui::GetIO().FontGlobalScale = scale;
}

void Application::handle_cell_click(float world_x, float world_y) {
	const float size = 8.0f;
	const float offset_x = 50.0f;
	const float offset_y = 50.0f;
	const float sqrt3 = std::sqrt(3.0f);

	// Quitar offset del grid
	float wx = world_x - offset_x;
	float wy = world_y - offset_y;

	// Conversión axial fraccional
	float q = (sqrt3 / 3.0f * wx - 1.0f / 3.0f * wy) / size;
	float r = (2.0f / 3.0f * wy) / size;

	// Convertir a cube para redondear
	float x = q;
	float z = r;
	float y = -x - z;

	float rx = std::round(x);
	float ry = std::round(y);
	float rz = std::round(z);

	float x_diff = std::abs(rx - x);
	float y_diff = std::abs(ry - y);
	float z_diff = std::abs(rz - z);

	if (x_diff > y_diff && x_diff > z_diff)
		rx = -ry - rz;
	else if (y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;

	int final_q = static_cast<int>(rx);
	int final_r = static_cast<int>(rz);

	if (m_automaton.in_bounds(final_q, final_r))
		m_automaton.toggle(final_q, final_r);
}
} // namespace cas