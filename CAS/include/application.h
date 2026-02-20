#pragma once
#include <core/gui_manager.h>
#include <automaton/hex_automaton.h>

#include <SDL3/SDL.h>
#include "../renderer.h"
#include <memory>
#include "../window.h"
#include "../sdl_context.h"

namespace cas {

class Application {
public:
	Application();
	~Application();

	void run();
private:
	void process_events();
	void update(double dt);
	void render();

private:
	SDLContext m_sdl{ SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	std::unique_ptr<platform::Window> m_window;
	std::unique_ptr<platform::Renderer> m_renderer;

	GuiManager m_gui;
	float m_simulation_speed = 10.0f;
	ImVec4 m_alive_color = ImVec4(0.0f, 0.78f, 0.47f, 1.0f);
	ImVec4 m_grid_color = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	ImVec4 m_background_color = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);

	HexAutomaton m_automaton{ 50, 40 };

	Camera m_camera;

	bool m_is_running = false;
	bool m_is_simulating = false;

};

} // namespace cas