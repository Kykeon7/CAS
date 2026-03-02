#pragma once

#include <core/gui_manager.h>
#include <imgui.h>

#include <memory>

#include <automaton/hex_automaton.h>
#include "platform/renderer.h"

#include "platform/window.h"
#include "platform/sdl_context.h"
#include "event/event_bus.h"
#include "core/graphics/color.h"
#include "camera.h"

namespace cas {

class Application {
public:
	Application();

	void run();
private:
	void process_events();
	void update(double dt);
	void render();
	void update_imgui_scale();
	void handle_cell_click(float world_x, float world_y);

private:
	SDLContext m_sdl{ SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	platform::Window m_window;
	platform::Renderer m_renderer;
	GuiManager m_gui;
	float m_simulation_speed = 10.0f;
	Color m_alive_color = { 0.0f, 0.78f, 0.47f, 1.0f };
	Color m_grid_color = { 0.25f, 0.25f, 0.25f, 1.0f };
	Color m_background_color = { 0.08f, 0.08f, 0.08f, 1.0f };

	HexAutomaton m_automaton;

	Camera m_camera;

	EventBus m_event_bus;

	bool m_is_running = false;
	bool m_is_simulating = false;
	float m_ui_scale = 1.0f;

	std::vector<float> m_population_history;

};

} // namespace cas