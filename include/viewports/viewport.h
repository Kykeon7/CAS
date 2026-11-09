
#pragma once

#include <inputs/input.h>
#include <viewports/camera.h>
#include <math/vec2.h>
#include <rendering/automaton/renderer.h>
#include <rendering/automaton/config.h>
#include <viewports/hex_layout.h>
#include <simulations/simulation.h>
#include <viewports/history_buffer.h>
#include <memory>
#include <optional>
#include <functional>
#include <string>
#include <vector>
#include <viewports/linear_history.h>

namespace cas::viewports {

class viewport {
public:
    explicit viewport(
        std::unique_ptr<cas::rendering::automaton::renderer> renderer = nullptr
    );

    void handle_input(
        const inputs::input& input,
        bool is_hovered,
        math::vec2f viewport_pos
    );

    void update_sim();

    [[nodiscard]]
    std::optional<std::reference_wrapper<platform::texture>>
    render();

    math::vec2f screen_to_world(const math::vec2f& local) const;

    cas::simulations::simulation& simulation() {
        return _simulation;
    }

    const cas::simulations::simulation& simulation() const {
        return _simulation;
    }

    hex_layout& layout() {
        return _layout;
    }

    cas::rendering::automaton::config& config() {
        return _config;
    }

    const cas::rendering::automaton::config& config() const {
        return _config;
    }

    cas::rendering::automaton::config& preview_config() {
        return _preview_config;
    }

    const cas::rendering::automaton::config& preview_config() const {
        return _preview_config;
    }

    history_buffer& population_history() {
        return _population_history;
    }

    const history_buffer& population_history() const {
        return _population_history;
    }

    history_buffer& density_history() {
        return _density_history;
    }

    const history_buffer& density_history() const {
        return _density_history;
    }

    history_buffer& activity_history() {
        return _activity_history;
    }

    const history_buffer& activity_history() const {
        return _activity_history;
    }

    [[nodiscard]]
    linear_history& linear_history_data() noexcept {
        return _linear_history;
    }

    [[nodiscard]]
    const linear_history& linear_history_data() const noexcept {
        return _linear_history;
    }

    cas::viewports::camera& camera() {
        return _camera;
    }

    const cas::viewports::camera& camera() const {
        return _camera;
    }

    void set_size(const math::vec2i& size);

    const std::string& name() const noexcept;
    void set_name(std::string name);

    void set_renderer(std::unique_ptr<cas::rendering::automaton::renderer> renderer);

    void zoom_in();
    void zoom_out();
    void zoom_reset();

    void reset_position();

private:
    void zoom_at(const math::vec2f& local, float factor);

private:
    cas::simulations::simulation _simulation;

    hex_layout _layout;
    std::unique_ptr<cas::rendering::automaton::renderer> _renderer;
    cas::rendering::automaton::config _config;
    cas::rendering::automaton::config _preview_config;

    cas::viewports::camera _camera;

    history_buffer _population_history;
    history_buffer _density_history;
    history_buffer _activity_history;
    linear_history _linear_history;

    std::string _name{ "Main" };
};

} // namespace cas::viewports
