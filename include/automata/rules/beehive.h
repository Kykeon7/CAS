#pragma once

#include <array>

#include <automata/rules/rule.h>

namespace cas::automata::rules {

/// @brief Implements the Beehive cellular automaton rule.
/// @details The rule supports three cell states: 0, 1, and 2.
///          Its transition behavior is defined by a lookup table
///          indexed by the number of neighboring cells in states
///          1 and 2, excluding the center cell.
///          The rule is designed for hexagonal neighborhoods
///          with up to six neighbors.
class beehive final : public rule {
public:
    beehive() = default;

    beehive(const beehive&) = delete;
    beehive& operator=(const beehive&) = delete;

    beehive(beehive&&) = delete;
    beehive& operator=(beehive&&) = delete;

    ~beehive() override = default;

    /// @brief Calculates the next state of a cell.
    /// @param stats Statistics describing the cell and its neighborhood.
    /// @return The resulting cell state.
    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    /// @brief Returns the number of states supported by this rule.
    /// @return Number of supported states.
    int state_count() const noexcept override;

private:
    static constexpr std::array<automata::cell_state, 28> _table{ {
        0, 1, 2, 1, 2, 0, 0,
        0, 2, 2, 2, 1, 1,
        0, 0, 2, 2, 0,
        0, 2, 2, 0,
        0, 0, 2,
        2, 0, 0
    } };
};

} // namespace cas::automata::rules