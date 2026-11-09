#pragma once

#include <automata/automaton.h>

namespace cas::simulations {

class simulation {
public:
    simulation();

    void step();

    void randomize(std::uint32_t seed);

    void randomize(
        std::span<const std::uint32_t> percentages,
        std::uint32_t seed
    );

    automata::automaton& automaton() {
        return _automaton;
    }
    const automata::automaton& automaton() const {
        return _automaton;
    }

    void set_speed(float speed) { _speed = speed; }
    float speed() const { return _speed; }

    void cycle_cell_state(int x, int y);
    void cycle_cell_state_axial(int q, int r);
    void toggle() { _running = !_running; }
    void resize(int width, int height);
    void clean();

    void set_boundary(automata::boundary_type boundary);
    void set_random_seed(std::uint32_t seed) noexcept;

    const automata::rules::rule& rule() const { return _automaton.rule(); }
    void set_rule(std::unique_ptr<automata::rules::rule> rule);

    int width() const { return _automaton.width(); }
    int height() const { return _automaton.height(); }

    bool is_running() const { return _running; }

    int state_count(automata::cell_state state) const {
        return _automaton.state_count(state);
    }

private:
    automata::automaton _automaton;
    float _speed = 10.0f;
    bool _running = false;
};

} // namespace cas::simulations