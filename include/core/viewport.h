#pragma once

#include <memory>

#include <core/input/input.h>
#include <core/rendering/renderer.h>
#include <automaton/topology.h>
#include <automaton/rule.h>
#include <automaton/automaton.h>
#include <core/rendering/layouts/hex_layout.h>
#include <core/rendering/automaton_renderers/automaton_renderer.h>
#include <core/rendering/automaton_renderers/render_config.h>
#include <core/simulation.h>

namespace cas {
class Viewport {
public:
    Viewport();
    void handle_input(const Input& input, bool is_hovered, Vec2 viewport_pos);
    void update_sim(float dt);
    void render(platform::Renderer& renderer);
    Vec2 screen_to_world(const Vec2& local) const;

    Simulation& simulation() { return m_simulation; }
    layouts::HexLayout& layout() { return m_layout; }
    AutomatonRenderConfig& config() { return m_config; }
    std::vector<float>& population_history() { return m_population_history; }
    std::vector<float>& density_history() { return m_density_history; }
    std::vector<float>& activity_history() { return m_activity_history; }
    Camera& camera() { return m_camera; }

private:
    Simulation m_simulation;

    layouts::HexLayout m_layout;
    std::unique_ptr<AutomatonRenderer> m_renderer;
    AutomatonRenderConfig m_config;

    Camera m_camera;    

    std::vector<float> m_population_history;
    std::vector<float> m_density_history;
    std::vector<float> m_activity_history;
};
}

