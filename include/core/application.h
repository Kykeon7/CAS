#pragma once

#include <core/gui/gui_manager.h>
#include <imgui.h>

#include <memory>

#include <automaton/hex_automaton.h>
#include "rendering/renderer.h"

#include "platform/window.h"
#include "platform/sdl_context.h"
#include "event/event_bus.h"
#include "core/graphics/color.h"
#include "camera.h"
#include <automaton/automaton.h>
#include <core/rendering/layouts/hex_layout.h>
#include <core/rendering/automaton_renderers/render_config.h>
#include <core/rendering/automaton_renderers/hex_renderer.h>
#include "rendering/automaton_renderers/automaton_renderer.h"
#include <core/gui/panels/gui_panel.h>
#include <core/graphics/colors.h>
#include "input/input.h"
#include <core/viewport.h>
#include <core/gui/layout_manager.h>

namespace cas {

template <typename T>
concept PanelType = std::derived_from<T, panels::Panel>;

class Application {
public:
	Application();

	void run();
private:
	void process_events();
	void update(float dt);
	void render();
	void update_imgui_scale();
	void set_active_viewport(size_t index);
	void render_viewport_tabs();

	template <PanelType T, typename... Args>
	T& add_panel(
		std::string_view window_name,
		Args&&... args
	)
	{
		auto panel =
			std::make_unique<T>(
				std::forward<Args>(args)...
			);

		auto& panel_ref = *panel;

		m_panels.emplace_back(std::move(panel));

		auto window =
			std::make_unique<Window>(
				std::string(window_name)
			);

		window->add_panel(panel_ref);

		m_layout.add_window(std::move(window));

		return panel_ref;
	}

private:
	SDLContext m_sdl{ SDL_INIT_VIDEO | SDL_INIT_EVENTS };
	platform::Window m_window;
	platform::Renderer m_renderer;
	GuiManager m_gui;

	size_t m_active_viewport = 0;
	size_t m_prev_active_viewport = 0;

	EventBus m_event_bus;

	Input m_input;

	bool m_is_running = false;
	float m_ui_scale = 1.0f;

	std::vector<std::unique_ptr<Viewport>> m_viewports;
	std::vector<std::unique_ptr<panels::Panel>> m_panels;

	LayoutManager m_layout;

};

} // namespace cas