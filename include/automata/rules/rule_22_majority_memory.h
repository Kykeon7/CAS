#pragma once

#include <automata/cell_state.h>
#include <automata/cell_stats.h>
#include <automata/rules/rule.h>

namespace cas::automata::rules {

/// @brief Implements a one-dimensional binary cellular automaton rule
///        based on Rule 22 with four generations of temporal memory.
/// @details The rule uses two cell states: dead (0) and alive (1).
///          Its transition behavior is based on a linear neighborhood
///          of three cells arranged on a Cartesian grid.
///          Before evaluating the transition, the automaton combines
///          the current configuration with the four previous generations
///          using a per-cell majority vote.
///          The resulting binary configuration is evaluated using
///          the Rule 22 transition condition.
class rule_22_majority_memory final : public rule {
public:
    rule_22_majority_memory() = default;

    rule_22_majority_memory(const rule_22_majority_memory&) = delete;
    rule_22_majority_memory& operator=(const rule_22_majority_memory&) = delete;

    rule_22_majority_memory(rule_22_majority_memory&&) = delete;
    rule_22_majority_memory& operator=(rule_22_majority_memory&&) = delete;

    ~rule_22_majority_memory() override = default;

    /// @brief Calculates the next state of a cell.
    /// @param stats Statistics describing the cell and its linear neighborhood
    ///              in the temporally summarized configuration.
    /// @return Alive (1) if exactly one cell is in state 1; dead (0) otherwise.
    cell_state next(
        const cell_stats& stats
    ) const override;

    /// @brief Returns the number of states supported by this rule.
    /// @return 2, representing dead and alive states.
    int state_count() const noexcept override;

    /// @brief Returns the number of previous generations required by this rule.
    /// @return 4 previous generations.
    int memory_depth() const noexcept override;
};

} // namespace cas::automata::rules