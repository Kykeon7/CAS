#include "core/application.h"

#include <print>
#include <core/time.h>
#include <core/input/input.h>
#include <algorithm>
#include "core/math/vec2.h"
#include <core/graphics/color_conv.h>
#include <event/sim_events.h>
#include <event/window_events.h>
#include <event/input_events.h>

#include <automaton/topologies.h>
#include <automaton/rules.h>
#include <core/rendering/layouts/hex_layout.h>
#include <core/rendering/automaton_renderers/hex_renderer.h>

#include <core/gui/panels/sim_panel.h>
#include <core/gui/panels/stats_panel.h>
#include <core/gui/panels/appearance_panel.h>
#include <core/gui/panels/graphs_panel.h>
#include <core/gui/panels/help_panel.h>
#include <core/gui/panels/view_panel.h>
#include <imgui_internal.h>

namespace cas {

    Application::Application()
        : m_window(
            "CAS - Cellular Automaton Simulator",
            800,
            600
        )
        , m_renderer(m_window.native())
        , m_gui(m_window, m_renderer)
    {
        m_viewports.emplace_back(std::make_unique<Viewport>());

        m_renderer.set_camera(
            &m_viewports[0]->camera()
        );

        m_is_running = true;

        std::srand(time(nullptr));

        add_panel<panels::Simulation>("Simulation", 
            *m_viewports[0]
        );

        add_panel<panels::Stats>("Stats", 
            m_viewports[0]->simulation().automaton()
        );

        add_panel<panels::Appearance>("Appearance",
            m_viewports[0]->config()
        );

        add_panel<panels::Graphs>("Graphs",
            *m_viewports[0],
            m_ui_scale
        );

        add_panel<panels::Help>("Help");

        add_panel<panels::ViewportPanel>("Viewport",
            *m_viewports[0],
            m_renderer,
            m_input
        );

        m_event_bus.subscribe<event::WindowFullScreenToggle>(
            [this](const event::WindowFullScreenToggle&) {
                bool fs = !m_window.is_full_screen();

                m_window.set_full_screen(fs);

                m_ui_scale = fs ? 1.5f : 1.0f;

                ImGui::GetIO().FontGlobalScale = m_ui_scale;
            }
        );

        m_event_bus.subscribe<event::Quit>(
            [this](const event::Quit&) {
                m_is_running = false;
            }
        );
    }

    void Application::run()
    {
        auto& time = cas::Time::instance();

        update_imgui_scale();

        while (m_is_running) {
            time.tick();

            m_input.update();

            auto delta_time = time.delta_time();

            process_events();
            update(delta_time);
            render();
        }
    }

    void Application::process_events()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT)
                m_is_running = false;

            if (event.type == SDL_EVENT_MOUSE_WHEEL)
                m_input.add_mouse_wheel(event.wheel.y);

            m_gui.process_event(event);
        }
    }

    void Application::update(float dt)
    {
        for (auto& viewport : m_viewports)
            viewport->update_sim(dt);

        if (m_active_viewport == m_prev_active_viewport)
            return;

        m_prev_active_viewport = m_active_viewport;

        auto index = m_active_viewport;
        auto& vp = *m_viewports[index];

        m_renderer.set_camera(
            &vp.camera()
        );

        for (auto& panel : m_panels)
            panel->set_viewport(vp);
    }

    void Application::render() {
        {
            auto frame = m_gui.begin_frame(m_renderer);

            m_gui.begin_dockspace();

            m_layout.render();

            m_gui.end_dockspace();
        }

        m_renderer.present();
    }

    void Application::update_imgui_scale()
    {
        float scale =
            SDL_GetWindowDisplayScale(m_window.native());

        if (scale <= 0.0f)
            scale = 1.0f;

        ImGui::GetIO().FontGlobalScale = scale;
    }

} // namespace cas