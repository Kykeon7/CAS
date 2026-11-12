#pragma once

#include <array>

#include <automata/cell_state.h>
#include <automata/cell_stats.h>
#include <automata/rules/rule.h>

namespace cas::automata::rules {

/// @brief Implements a three-state cellular automaton rule.
/// @details The rule supports three cell states: 0, 1, and 2.
///          Its transition behavior is defined by an 8x8 lookup table
///          indexed by the total number of cells in states 1 and 2,
///          including the center cell.
///          The rule is designed for hexagonal neighborhoods
///          with up to six neighbors.
class spiral final : public rule {
public:
    spiral() = default;

    spiral(const spiral&) = delete;
    spiral& operator=(const spiral&) = delete;

    spiral(spiral&&) = delete;
    spiral& operator=(spiral&&) = delete;

    ~spiral() override = default;

    /// @brief Calculates the next state of a cell.
    /// @param stats Statistics describing the cell and its neighborhood.
    /// @return The resulting cell state.
    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    /// @brief Returns the number of states supported by this rule.
    /// @return 3, representing states 0, 1, and 2.
    int state_count() const noexcept override;

private:
    static constexpr std::array<
        std::array<automata::cell_state, 8>,
        8
    > _table{ {
        // state_2 = 0
        {{ 0, 1, 2, 1, 2, 2, 2, 0 }},

        // state_2 = 1
        {{ 0, 2, 2, 1, 2, 2, 2, 0 }},

        // state_2 = 2
        {{ 0, 0, 2, 2, 1, 2, 0, 0 }},

        // state_2 = 3
        {{ 0, 2, 2, 1, 2, 0, 0, 0 }},

        // state_2 = 4
        {{ 0, 0, 2, 1, 0, 0, 0, 0 }},

        // state_2 = 5
        {{ 0, 0, 2, 0, 0, 0, 0, 0 }},

        // state_2 = 6
        {{ 0, 0, 0, 0, 0, 0, 0, 0 }},

        // state_2 = 7
        {{ 0, 0, 0, 0, 0, 0, 0, 0 }}
    } };
};

} // namespace cas::automata::rules