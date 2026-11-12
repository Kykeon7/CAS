#include <viewports/viewport.h>

#include <algorithm>
#include <utility>
#include <stdexcept>

#include <automata/rules/factory.h>
#include <automata/topologies.h>
#include <rendering/automaton/hex_renderer.h>
#include <system/logger.h>
#include <system/profiler.h>
#include <system/time.h>

namespace cas::viewports {

viewport::viewport(
    std::unique_ptr<cas::rendering::automaton::renderer> renderer
)
    : _renderer(std::move(renderer))
{
}


void viewport::handle_input(
    const inputs::input& input,
    bool is_hovered,
    math::vec2f viewport_pos
) {
    if (!is_hovered)
        return;

    math::vec2f mouse = input.mouse_position();

    math::vec2f local{
        mouse.x - viewport_pos.x,
        mouse.y - viewport_pos.y
    };

    if (input.is_mouse_pressed(inputs::mouse_button::left)) {
        const math::vec2f world = screen_to_world(local);

        const auto [q, r] = _layout.world_to_hex(
            world.x,
            world.y,
            _config.cell_size,
            _config.offset.x,
            _config.offset.y
        );

        _simulation.cycle_cell_state_axial(q, r);
    }

    if (input.is_mouse_held(inputs::mouse_button::right)) {
        math::vec2f delta = input.mouse_delta();

        _camera.set_position(
            _camera.position() - delta / _camera.zoom()
        );
    }

    const auto wheel = input.mouse_wheel();

    if (wheel > 0.0f) {
        zoom_at(local, 1.1f);
    }
    else if (wheel < 0.0f) {
        zoom_at(local, 1.0f / 1.1f);
    }
}


void viewport::update_sim() {
    auto& time = time::instance();

    if (!_simulation.is_running())
        return;

    if (time.every(1 / _simulation.speed(), 0)) {

        auto& profiler = system::profiler::instance();

        profiler.begin(system::profile_id::update);
        _simulation.step();
        profiler.end(system::profile_id::update);

        _linear_history.record(_simulation.automaton());

        const float population = static_cast<float>(
            _simulation.state_count(1)
            );

        const float total = static_cast<float>(
            _simulation.width() * _simulation.height()
            );

        const float density =
            total > 0.0f ? population / total : 0.0f;

        // Recuperar la muestra anterior antes de insertar la nueva.
        const auto previous_samples =
            _population_history.samples();

        const bool has_previous = !previous_samples.empty();

        const float previous_population = has_previous
            ? previous_samples.back()
            : 0.0f;

        _population_history.push(population);
        _density_history.push(density);

        if (has_previous) {
            _activity_history.push(
                std::abs(population - previous_population)
            );
        }
    }
}


std::optional<std::reference_wrapper<platform::texture>>
viewport::render() {
    if (!_renderer)
        throw std::invalid_argument(
            "Cannot render viewport without a renderer."
        );

    _linear_history.record(_simulation.automaton());

    return _renderer->render(
        _simulation.automaton(),
        _linear_history,
        _config,
        _camera
    );
}

math::vec2f viewport::screen_to_world(const math::vec2f& local) const {
    const math::vec2f size{ _camera.viewport_size() };
    const float zoom = _camera.zoom();
    const math::vec2f position = _camera.position();

    return {
        (local.x - size.x * 0.5f) / zoom + position.x,
        (local.y - size.y * 0.5f) / zoom + position.y
    };
}

void viewport::set_renderer(
    std::unique_ptr<cas::rendering::automaton::renderer> renderer
) {
    if (!renderer)
        return;

    _renderer = std::move(renderer);

    const auto& size = _camera.viewport_size();

    if (size.x > 0 && size.y > 0)
        _renderer->resize(size);
}

void viewport::zoom_in() {
    const auto& size = _camera.viewport_size();

    zoom_at(
        { size.x * 0.5f, size.y * 0.5f },
        1.1f
    );
}

void viewport::zoom_out() {
    const auto& size = _camera.viewport_size();

    zoom_at(
        { size.x * 0.5f, size.y * 0.5f },
        1.0f / 1.1f
    );
}

void viewport::zoom_reset() {
    _camera.set_zoom(1.0f);
}

void viewport::reset_position() {
    _camera.set_position({ 0.0f, 0.0f });
}

void viewport::zoom_at(const math::vec2f& local, float factor) {
    const math::vec2f size{ _camera.viewport_size() };

    const math::vec2f screen_center{
        size.x * 0.5f,
        size.y * 0.5f
    };

    const math::vec2f world_before = screen_to_world(local);

    const float new_zoom = std::clamp(
        _camera.zoom() * factor,
        0.1f,
        10.0f
    );

    _camera.set_zoom(new_zoom);

    _camera.set_position({
        world_before.x
            - (local.x - screen_center.x) / new_zoom,
        world_before.y
            - (local.y - screen_center.y) / new_zoom
    });
}

const std::string& viewport::name() const noexcept {
    return _name;
}

void viewport::set_name(std::string name) {
    if (!name.empty())
        _name = std::move(name);
}

void viewport::set_size(const math::vec2i& size) {
    _camera.set_viewport_size(size);

    if (_renderer)
        _renderer->resize(size);
}

} // namespace cas::viewports