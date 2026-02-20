#include "application.h"	

#include <print>
#include <core/time.h>
#include <core/input.h>


namespace cas {

Application::Application()
	: m_automaton(50, 40) {

	auto* driver = SDL_GetCurrentVideoDriver();
	std::println(
		"Current video driver: {}",
		std::string_view(driver ? driver : "null")
	);

	std::println("SDL version linked: {}.{}.{}",
		SDL_MAJOR_VERSION,
		SDL_MINOR_VERSION,
		SDL_MICRO_VERSION
	);


	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		std::println("SDL_Init failed: {}", SDL_GetError());
		return;
	}

	std::println("Error after init: '{}'", SDL_GetError());

	driver = SDL_GetCurrentVideoDriver();
	std::println(
		"Current video driver after init: {}",
		std::string_view(driver ? driver : "null")
	);

	m_window = SDL_CreateWindow(
		"CAS - Cellular Automaton Simulator",
		800,
		600,
		0
	);

	if (!m_window) {
		std::println("SDL_CreateWindow failed: {}", SDL_GetError());
		return;
	}

	SDL_SetWindowResizable(m_window, true);

	m_renderer = SDL_CreateRenderer(m_window, nullptr);

	if (!m_renderer) {
		std::println("SDL_CreateRenderer failed: {}", SDL_GetError());
		return;
	}

	m_gui.init(m_window, m_renderer);

	m_automaton.randomize();

	m_is_running = true;
}

Application::~Application() {
	m_gui.shutdown();

	if (m_renderer) SDL_DestroyRenderer(m_renderer);
	if (m_window) SDL_DestroyWindow(m_window);

	SDL_Quit();
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
	if (m_is_simulating)
		m_automaton.step();
}


void Application::render() {
	SDL_SetRenderDrawColor(m_renderer, 20, 20, 20, 255);
	SDL_RenderClear(m_renderer);

	// ===== Dibujar automata =====

	const int cell_size = 12;

	for (int r = 0; r < m_automaton.height(); ++r) {
		for (int q = 0; q < m_automaton.width(); ++q) {

			const auto& cell = m_automaton.cell(q, r);

			if (!cell.alive)
				continue;

			SDL_FRect rect{
				static_cast<float>(q * cell_size),
				static_cast<float>(r * cell_size),
				static_cast<float>(cell_size),
				static_cast<float>(cell_size)
			};

			SDL_SetRenderDrawColor(m_renderer, 0, 200, 120, 255);
			SDL_RenderFillRect(m_renderer, &rect);
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

	m_gui.end_frame(m_renderer);

	SDL_RenderPresent(m_renderer);
}


} // namespace cas