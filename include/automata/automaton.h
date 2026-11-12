#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include <automata/cell_state.h>
#include <automata/grid_storage.h>
#include <automata/topology.h>

namespace cas::automata {

namespace rules {
    class rule;
}

/// @brief Manages the state and evolution of a cellular automaton.
class automaton {

public:
    /// @brief Constructs an automaton with the specified configuration.
    /// @param width Requested grid width.
    /// @param height Requested grid height.
    /// @param topology Topology used to determine cell neighborhoods.
    /// @param rule Rule used to calculate cell transitions.
    automaton(
        int width,
        int height,
        automata::topology topology,
        std::unique_ptr<rules::rule> rule
    );

    /// @brief Constructs an automaton with the default configuration.
    automaton();

    automaton(const automaton&) = delete;
    automaton& operator=(const automaton&) = delete;

    automaton(automaton&&);
    automaton& operator=(automaton&&);

    ~automaton();

    /// @brief Advances the automaton by one generation.
    void step();

    /// @brief Randomizes the grid using an approximately uniform distribution.
    /// @param seed Seed used by the random number generator.
    void randomize(std::uint32_t seed);

    /// @brief Randomizes the grid using the specified state distribution.
    /// @param percentages Percentage assigned to each state.
    /// @param seed Seed used by the random number generator.
    void randomize(
        std::span<const std::uint32_t> percentages,
        std::uint32_t seed
    );

    /// @brief Checks whether the specified coordinates are inside the grid.
    /// @param x Horizontal coordinate.
    /// @param y Vertical coordinate.
    /// @return True if the coordinates are inside the grid.
    [[nodiscard]] bool in_bounds(int x, int y) const;

    /// @brief Returns the state of the cell at the specified coordinates.
    /// @param x Horizontal coordinate.
    /// @param y Vertical coordinate.
    /// @return Cell state, or the default-constructed state if out of bounds.
    [[nodiscard]] cell_state get(int x, int y) const;

    /// @brief Changes the state of the cell at the specified coordinates.
    /// @param x Horizontal coordinate.
    /// @param y Vertical coordinate.
    /// @param state New cell state.
    /// @note Does nothing if the coordinates are outside the grid.
    void set(int x, int y, cell_state state);

    /// @brief Resizes the grid and clears its current state.
    /// @param width Requested grid width.
    /// @param height Requested grid height.
    void resize(int width, int height);

    /// @brief Restores a previously saved grid configuration.
    /// @param cells Cell states to restore.
    /// @param generation_count Generation associated with the configuration.
    void restore_state(
        std::span<const cell_state> cells,
        int generation_count
    );

    /// @brief Returns the current grid width.
    [[nodiscard]] int width() const noexcept { return _storage.width(); }

    /// @brief Returns the current grid height.
    [[nodiscard]] int height() const noexcept { return _storage.height(); }

    /// @brief Replaces the current transition rule.
    /// @param rule New transition rule.
    void set_rule(std::unique_ptr<rules::rule> rule);

    /// @brief Replaces the current topology.
    /// @param topology New topology.
    void set_topology(const automata::topology& topology);

    /// @brief Changes the boundary behavior of the topology.
    /// @param boundary New boundary behavior.
    void set_boundary(boundary_type boundary);

    /// @brief Clears the grid and resets the simulation.
    void clean();

    /// @brief Returns the current transition rule.
    [[nodiscard]] const rules::rule& rule() const noexcept;

    /// @brief Returns the current topology.
    [[nodiscard]] const automata::topology& topology() const noexcept {
        return _topology;
    }

    /// @brief Returns the underlying grid storage.
    [[nodiscard]] const grid_storage& storage() const noexcept {
        return _storage;
    }

    /// @brief Returns the number of cells in the specified state.
    /// @param state State to count.
    /// @return Number of cells in that state.
    [[nodiscard]] int state_count(cell_state state) const noexcept;

    /// @brief Returns the current generation number.
    [[nodiscard]] int generation_count() const noexcept {
        return _generation_count;
    }

    /// @brief Returns the revision of the current configuration.
    [[nodiscard]] std::uint64_t configuration_revision() const noexcept {
        return _configuration_revision;
    }

private:
    void invalidate_configuration() noexcept;
    void reset_history();
    void clear_state();

private:
    grid_storage _storage;
    automata::topology _topology;
    std::unique_ptr<rules::rule> _rule;

    int _requested_width;
    int _requested_height;

    std::vector<int> _state_counts;
    int _generation_count = 0;

    std::vector<std::vector<cell_state>> _history;
    std::vector<cell_state> _memory_summary;
    bool _history_valid = false;

    std::uint64_t _configuration_revision = 0;
};

} // namespace cas::automata
