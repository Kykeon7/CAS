#pragma once

#include <cstdint>
#include <memory>
#include <span>

#include <automata/automaton.h>

namespace cas::simulations {

/**
 * @brief Coordinates the execution and configuration of a cellular automaton simulation.
 *
 * Manages the automaton, its execution speed, and its running state.
 * Provides operations for advancing, randomizing, resizing, and modifying
 * the automaton.
 */
class simulation {
public:
    /**
     * @brief Constructs a simulation with its default automaton configuration.
     *
     * Creates a 1000x1000 automaton using a finite hexagonal topology
     * and the spiral rule.
     */
    simulation();

    /**
     * @brief Advances the automaton by one generation.
     */
    void step();

    /**
     * @brief Randomizes the automaton using the specified seed.
     *
     * @param seed Seed used by the random number generator.
     */
    void randomize(std::uint32_t seed);

    /**
     * @brief Randomizes the automaton using the specified state percentages and seed.
     *
     * @param percentages Percentage distribution used to initialize cell states.
     * @param seed Seed used by the random number generator.
     */
    void randomize(
        std::span<const std::uint32_t> percentages,
        std::uint32_t seed
    );

    /**
     * @brief Provides mutable access to the underlying automaton.
     *
     * @return Reference to the managed automaton.
     */
    automata::automaton& automaton() {
        return _automaton;
    }

    /**
     * @brief Provides read-only access to the underlying automaton.
     *
     * @return Constant reference to the managed automaton.
     */
    const automata::automaton& automaton() const {
        return _automaton;
    }

    /**
     * @brief Sets the simulation speed.
     *
     * @param speed Number of generations per second.
     */
    void set_speed(float speed) { _speed = speed; }

    /**
     * @brief Returns the configured simulation speed.
     *
     * @return Number of generations per second.
     */
    float speed() const { return _speed; }

    /**
     * @brief Advances the state of a cell to its next state.
     *
     * Does nothing if the specified coordinates are outside the automaton.
     *
     * @param x Horizontal cell coordinate.
     * @param y Vertical cell coordinate.
     */
    void cycle_cell_state(int x, int y);

    /**
     * @brief Advances the state of a cell using axial hexagonal coordinates.
     *
     * Converts the axial coordinates to the automaton's grid coordinates
     * before cycling the cell state.
     *
     * @param q Axial horizontal coordinate.
     * @param r Axial vertical coordinate.
     */
    void cycle_cell_state_axial(int q, int r);

    /**
     * @brief Toggles the simulation's running state.
     */
    void toggle() { _running = !_running; }

    /**
     * @brief Resizes the automaton.
     *
     * Does nothing if either dimension is less than one.
     *
     * @param width New automaton width in cells.
     * @param height New automaton height in cells.
     */
    void resize(int width, int height);

    /**
     * @brief Resets the automaton to its clean state.
     */
    void clean();

    /**
     * @brief Sets the boundary condition of the automaton's topology.
     *
     * @param boundary Boundary condition to apply.
     */
    void set_boundary(automata::boundary_type boundary);

    /**
     * @brief Returns the rule currently used by the automaton.
     *
     * @return Constant reference to the active rule.
     */
    const automata::rules::rule& rule() const { return _automaton.rule(); }

    /**
     * @brief Replaces the automaton's rule and adjusts its topology.
     *
     * Selects a finite linear topology for the majority-memory rule
     * and a finite hexagonal topology for other rules. Preserves the
     * boundary condition of the current topology.
     *
     * Does nothing if the supplied rule is null.
     *
     * @param rule Rule to take ownership of.
     */
    void set_rule(std::unique_ptr<automata::rules::rule> rule);

    /**
     * @brief Returns the automaton width in cells.
     */
    int width() const { return _automaton.width(); }

    /**
     * @brief Returns the automaton height in cells.
     */
    int height() const { return _automaton.height(); }

    /**
     * @brief Checks whether the simulation is running.
     */
    bool is_running() const { return _running; }

    /**
     * @brief Returns the number of cells in the specified state.
     *
     * @param state Cell state to count.
     * @return Number of cells matching the specified state.
     */
    int state_count(automata::cell_state state) const {
        return _automaton.state_count(state);
    }

private:
    automata::automaton _automaton;
    float _speed = 10.0f;
    bool _running = false;
};

} // namespace cas::simulations
