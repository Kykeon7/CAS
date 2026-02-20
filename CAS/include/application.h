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
	
	bool m_is_running = false;

	SDLContext m_sdl{ SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	std::unique_ptr<platform::Window> m_window;
	std::unique_ptr<platform::Renderer> m_renderer;

	GuiManager m_gui;
	HexAutomaton m_automaton{ 50, 40 };

	bool m_is_simulating = false;

};

} // namespace cas