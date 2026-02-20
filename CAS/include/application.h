#pragma once
#include <core/gui_manager.h>
#include <automaton/hex_automaton.h>

#include <SDL3/SDL.h>

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

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	GuiManager m_gui;

	HexAutomaton m_automaton{ 50, 40 };

	bool m_is_simulating = false;

};

} // namespace cas