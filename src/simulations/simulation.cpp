#include <simulations/simulation.h>

#include <cmath>
#include <stdexcept>
#include <utility>

#include <automata/rules/factory.h>
#include <automata/rules/binary.h>
#include <automata/rules/rule_22_majority_memory.h>
#include <automata/topologies.h>

namespace cas::simulations {

simulation::simulation()
    : _automaton(
        1000,
        1000,
        automata::topologies::hex_finite(),
        automata::rules::spiral()
    )
{
}

void simulation::step() {
    _automaton.step();
}

int simulation::pending_steps(
    double delta_time,
    int max_steps
) {
    if (!_running)
        return 0;

    return _step_accumulator.advance(
        delta_time,
        1.0 / static_cast<double>(_speed),
        max_steps
    );
}

void simulation::reset_timing() noexcept {
    _step_accumulator.reset();
}

void simulation::set_speed(float speed) {
    if (!std::isfinite(speed) || speed <= 0.0f)
        throw std::invalid_argument(
            "Simulation speed must be finite and positive."
        );

    if (_speed == speed)
        return;

    _speed = speed;
    reset_timing();
}

void simulation::toggle() {
    _running = !_running;
    reset_timing();
}

void simulation::randomize(std::uint32_t seed) {
    _automaton.randomize(seed);
}

void simulation::randomize(
    std::span<const std::uint32_t> percentages,
    std::uint32_t seed
) {
    _automaton.randomize(percentages, seed);
}

void simulation::cycle_cell_state(int x, int y) {
    if (!_automaton.in_bounds(x, y))
        return;

    const automata::cell_state state = _automaton.get(x, y);
    const automata::cell_state state_count = _automaton.rule().state_count();

    _automaton.set(
        x,
        y,
        (state + 1) % state_count
    );
}

void simulation::cycle_cell_state_axial(int q, int r) {
    const int x = q + _automaton.width() / 2;
    const int y = r + _automaton.height() / 2;

    cycle_cell_state(x, y);
}

void simulation::resize(int width, int height) {
    if (width < 1 || height < 1)
        return;

    _automaton.resize(width, height);
    reset_timing();
}

void simulation::clean() {
    _automaton.clean();
    reset_timing();
}

void simulation::set_boundary(automata::boundary_type boundary) {
    _automaton.set_boundary(boundary);
    reset_timing();
}

void simulation::set_rule(std::unique_ptr<automata::rules::rule> rule) {
    if (!rule)
        return;

    const automata::boundary_type boundary =
        _automaton.topology().boundary();

    automata::topology new_topology =
        dynamic_cast<const class automata::rules::rule_22_majority_memory*>(rule.get())
        ? automata::topologies::linear_finite()
        : dynamic_cast<const automata::rules::binary*>(rule.get())
        ? automata::topologies::moore_finite()
        : automata::topologies::hex_finite();

    new_topology.set_boundary(boundary);

    _automaton.set_topology(new_topology);
    _automaton.set_rule(std::move(rule));

    reset_timing();
}

} // namespace cas::simulations